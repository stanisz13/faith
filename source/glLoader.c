#include "glLoader.h"

typedef GLXContext (*glXCreateContextAttribsARBProc)
(Display*, GLXFBConfig, GLXContext, Bool, const int*);


unsigned isExtensionSupported(const char *extList, const char *extension)
{
    //copied from :
    // https://www.khronos.org/opengl/wiki/Tutorial:_OpenGL_3.0_Context_Creation_(GLX)
    // ---
    // Helper to check for extension string presence.  Adapted from:
    //   http://www.opengl.org/resources/features/OGLextensions/
    
    const char *start;
    const char *where, *terminator;
  
    where = strchr(extension, ' ');
    if (where || *extension == '\0')
        return 0;

    for (start=extList;;)
    {
        where = strstr(start, extension);

        if (!where)
            break;

        terminator = where + strlen(extension);

        if (where == start || *(where - 1) == ' ')
            if (*terminator == ' ' || *terminator == '\0')
                return 1;

        start = terminator;
    }

    return 0;
}

int ctxErrorHandler(Display *dpy, XErrorEvent *ev)
{
    exit(0);
    return 0;
}

void configureOpenGL(ContextData* cdata)
{
    // Open a comminication to X server with default screen name.
    cdata->display = XOpenDisplay(NULL);
  
    if (!cdata->display)
    {
        logError("Unable to start communication with X server!");
        exit(0);
    }

    // Get a matching framebuffer config
     int visual_attribs[] =
        {
            GLX_X_RENDERABLE    , 1,
            GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
            GLX_RENDER_TYPE     , GLX_RGBA_BIT,
            GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
            GLX_RED_SIZE        , 8,
            GLX_GREEN_SIZE      , 8,
            GLX_BLUE_SIZE       , 8,
            GLX_ALPHA_SIZE      , 8,
            GLX_DEPTH_SIZE      , 24,
            GLX_STENCIL_SIZE    , 8,
            GLX_DOUBLEBUFFER    , 1,
            //GLX_SAMPLE_BUFFERS  , 1,
            //GLX_SAMPLES         , 4,
            None
        };

    unsigned glx_major, glx_minor;
 
    // Check if the version is not less than minimal
    if (!glXQueryVersion(cdata->display, &glx_major, &glx_minor))
    {
        logError("Could not obtain GLX drivers version!");
        exit(0);
    }

    printf("Your version of GLX drivers is: %u.%u\n", glx_major, glx_minor);
    
    if (glx_major < cdata->minimalGLXVersionMajor
        || glx_minor < cdata->minimalGLXVersionMinor)
    {
        logError("Your version of GLX drivers does not match the minimum requirements!");
        exit(0);
    }

    int fbcount;
    GLXFBConfig* fbc = glXChooseFBConfig(cdata->display, DefaultScreen(cdata->display),
                                         visual_attribs, &fbcount);
    if (!fbc)
    {
        logError("Failed to retrieve any framebuffer config");
        exit(1);
    }
    
    printf("Found %d matching framebuffer configs\n", fbcount);

    // Pick the framebuffer config/visual with the most samples per pixel
    int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;
    int i;
    
    for (i=0; i<fbcount; ++i)
    {
        XVisualInfo *vi = glXGetVisualFromFBConfig(cdata->display, fbc[i]);
        if (vi)
        {
            int samp_buf, samples;
            glXGetFBConfigAttrib(cdata->display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
            glXGetFBConfigAttrib(cdata->display, fbc[i], GLX_SAMPLES, &samples);
      
            printf("  Matching fbconfig %d, visual ID 0x%2x: SAMPLE_BUFFERS = %d,"
                    " SAMPLES = %d\n", 
                    i, vi->visualid, samp_buf, samples);

            if (best_fbc < 0 || samp_buf && samples > best_num_samp)
                best_fbc = i, best_num_samp = samples;
            if (worst_fbc < 0 || !samp_buf || samples < worst_num_samp)
                worst_fbc = i, worst_num_samp = samples;
        }
        
        XFree(vi);
    }

    GLXFBConfig bestFbc = fbc[best_fbc];

    XFree(fbc);

    XVisualInfo *vi = glXGetVisualFromFBConfig(cdata->display, bestFbc);
    printf("Chosen visual ID = 0x%x\n", vi->visualid);

    XSetWindowAttributes swa;
    swa.colormap = cdata->cmap = XCreateColormap(cdata->display,
                                           RootWindow(cdata->display, vi->screen), 
                                           vi->visual, AllocNone);
    swa.background_pixmap = None;
    swa.border_pixel = 0;
    swa.event_mask = StructureNotifyMask;

    cdata->window = XCreateWindow(cdata->display, RootWindow(cdata->display, vi->screen), 
                                0, 0, cdata->windowWidth, cdata->windowHeight,
                                0, vi->depth, InputOutput, 
                                vi->visual, 
                                CWBorderPixel|CWColormap|CWEventMask, &swa);
    if (!cdata->window)
    {
        logError("Failed to create X window!");
        exit(0);
    }

    XFree(vi);

    XStoreName(cdata->display, cdata->window, cdata->name);

    XMapWindow(cdata->display, cdata->window);

    const char *glxExts = glXQueryExtensionsString(cdata->display,
                                                   DefaultScreen(cdata->display));

    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
        glXGetProcAddressARB((const GLubyte *)"glXCreateContextAttribsARB");

    int (*oldHandler)(Display*, XErrorEvent*) =
        XSetErrorHandler(&ctxErrorHandler);

    // Check for the GLX_ARB_create_context extension string and the function.
    if (!isExtensionSupported(glxExts, "GLX_ARB_create_context") ||
         !glXCreateContextAttribsARB)
    {
        logError( "glXCreateContextAttribsARB() not found!");
        exit(0);    
    }
    else
    {
        int context_attribs[] =
            {
                GLX_CONTEXT_MAJOR_VERSION_ARB, cdata->minimalGLVersionMajor,
                GLX_CONTEXT_MINOR_VERSION_ARB, cdata->minimalGLVersionMinor,
                //GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
                None
            };

        cdata->ctx = glXCreateContextAttribsARB(cdata->display, bestFbc, 0,
                                          1, context_attribs);

        // Sync to ensure any errors generated are processed.
        XSync(cdata->display, False);
        
        if (cdata->ctx)
            printf("Created GL %u.%u context\n", cdata->minimalGLVersionMajor, cdata->minimalGLVersionMinor);
        else
        {
            logError("Failed to create GL %u.%u context!");
            exit(0);
        }
    }

    // Sync to ensure any errors generated are processed.
    XSync(cdata->display, 0);

    // Restore the original error handler
    XSetErrorHandler(oldHandler);

    if (!cdata->ctx)
    {
        logError("Failed to create an OpenGL context!");
        exit(0);
    }

    // Verifying that context is a direct context
    if (!glXIsDirect(cdata->display, cdata->ctx))
    {
        logS("Indirect GLX rendering context obtained");
    }
    else
    {
        logS("Direct GLX rendering context obtained");
    }

    glXMakeCurrent(cdata->display, cdata->window, cdata->ctx);

}

void clearConfigurationOfOpenGL(ContextData* cdata)
{
    glXMakeCurrent(cdata->display, 0, 0);
    glXDestroyContext(cdata->display, cdata->ctx);

    XDestroyWindow(cdata->display, cdata->window);
    XFreeColormap(cdata->display, cdata->cmap);
    XCloseDisplay(cdata->display);
}

