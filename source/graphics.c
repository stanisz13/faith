#include "graphics.h"

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092

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

typedef enum 
{
    MESA_V_SYNC = 1,
    EXT_V_SYNC = 2,
    SGI_V_SYNC = 4
    
} UserVSyncDataMasks;

void configureOpenGL(ContextData* cdata, UserVSyncData* udata)
{
    // Open a comminication to X server with default screen name.
    cdata->display = XOpenDisplay(NULL);

#if 0
    XSynchronize(cdata->display, 0);
#else
    XSynchronize(cdata->display, 1);
#endif
    
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

    int glx_major, glx_minor;
 
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
        exit(0);
    }
    
#if 0
    printf("Found %d matching framebuffer configs\n", fbcount);
#endif

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
#if 0     
            printf("  Matching fbconfig %d, visual ID 0x%2x: SAMPLE_BUFFERS = %d,"
                   " SAMPLES = %d\n", 
                   i, vi->visualid, samp_buf, samples);
#endif
            if (best_fbc < 0 || (samp_buf && samples > best_num_samp))
                best_fbc = i, best_num_samp = samples;
            if (worst_fbc < 0 || (!samp_buf || samples < worst_num_samp))
                worst_fbc = i, worst_num_samp = samples;
        }
        
        XFree(vi);
    }

    GLXFBConfig bestFbc = fbc[best_fbc];

    XFree(fbc);

    XVisualInfo *vi = glXGetVisualFromFBConfig(cdata->display, bestFbc);

#if 0
    printf("Chosen visual ID = 0x%x\n", vi->visualid);
#endif
    
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

    //logS(glxExts);
     
    // Check for the GLX_ARB_create_context extension string and the function.
    if (!isExtensionSupported(glxExts, "GLX_ARB_create_context") ||
         !glXCreateContextAttribsARB)
    {
        logError("glXCreateContextAttribsARB() not found!");
        exit(0);    
    }
    else
    {
        if (isExtensionSupported(glxExts, "GLX_MESA_swap_control"))
        {
            udata->mask |= MESA_V_SYNC;
        }
        if (isExtensionSupported(glxExts, "GLX_EXT_swap_control_tear"))
        {
            udata->mask |= EXT_V_SYNC;
        }
        if (isExtensionSupported(glxExts, "GLX_SGI_swap_control"))
        {
            udata->mask |= SGI_V_SYNC;
        }

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

#if 0
    // Verifying that context is a direct context
    if (!glXIsDirect(cdata->display, cdata->ctx))
    {
        logS("Indirect GLX rendering context obtained");
    }
    else
    {
        logS("Direct GLX rendering context obtained");
    }
#endif

    cdata->deleteMessage = XInternAtom(cdata->display, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(cdata->display, cdata->window, &cdata->deleteMessage, 1);

    glXMakeCurrent(cdata->display, cdata->window, cdata->ctx);
}

void freeContextData(ContextData* cdata)
{
    glXMakeCurrent(cdata->display, 0, 0);
    glXDestroyContext(cdata->display, cdata->ctx);

    XDestroyWindow(cdata->display, cdata->window);
    XFreeColormap(cdata->display, cdata->cmap);
    XCloseDisplay(cdata->display);
}

void loadFunctionPointers()
{
    //NOTE(Stanisz13): FRAMEBUFFERS
    glGenFramebuffers_FA = (PFNGLGENFRAMEBUFFERSPROC)glXGetProcAddress((const unsigned char*)"glGenFramebuffers");
    glBindFramebuffer_FA = (PFNGLBINDFRAMEBUFFERPROC)glXGetProcAddress((const unsigned char*)"glBindFramebuffer");
    glFramebufferTexture2D_FA = (PFNGLFRAMEBUFFERTEXTURE2DPROC)glXGetProcAddress((const unsigned char*)"glFramebufferTexture2D");

    //NOTE(Stanisz13): SHADERS
    glCreateShader_FA = (PFNGLCREATESHADERPROC)glXGetProcAddress((const unsigned char*)"glCreateShader");
    glShaderSource_FA = (PFNGLSHADERSOURCEPROC)glXGetProcAddress((const unsigned char*)"glShaderSource");
    glCompileShader_FA = (PFNGLCOMPILESHADERPROC)glXGetProcAddress((const unsigned char*)"glCompileShader");
    glGetShaderiv_FA = (PFNGLGETSHADERIVPROC)glXGetProcAddress((const unsigned char*)"glGetShaderiv");
    glGetShaderInfoLog_FA = (PFNGLGETSHADERINFOLOGPROC)glXGetProcAddress((const unsigned char*)"glGetShaderInfoLog");
    glAttachShader_FA = (PFNGLATTACHSHADERPROC)glXGetProcAddress((const unsigned char*)"glAttachShader");
    glDeleteShader_FA = (PFNGLDELETESHADERPROC)glXGetProcAddress((const unsigned char*)"glDeleteShader");

    //NOTE(Stanisz13): UNIFORMS
    glGetUniformLocation_FA = (PFNGLGETUNIFORMLOCATIONPROC)glXGetProcAddress((const unsigned char*)"glGetUniformLocation");
    glUniform1f_FA = (PFNGLUNIFORM1FPROC)glXGetProcAddress((const unsigned char*)"glUniform1f");
        
    //NOTE(Stanisz13): PROGRAMS
    glCreateProgram_FA = (PFNGLCREATEPROGRAMPROC)glXGetProcAddress((const unsigned char*)"glCreateProgram");
    glLinkProgram_FA = (PFNGLLINKPROGRAMPROC)glXGetProcAddress((const unsigned char*)"glLinkProgram");
    glGetProgramiv_FA = (PFNGLGETPROGRAMIVPROC)glXGetProcAddress((const unsigned char*)"glGetProgramiv");
    glGetProgramInfoLog_FA = (PFNGLGETPROGRAMINFOLOGPROC)glXGetProcAddress((const unsigned char*)"glGetProgramInfoLog");
    glDeleteProgram_FA = (PFNGLDELETEPROGRAMPROC)glXGetProcAddress((const unsigned char*)"glDeleteProgram_FA");

    //NOTE(Stanisz13): BUFFERS
    glGenVertexArrays_FA = (PFNGLGENVERTEXARRAYSPROC)glXGetProcAddress((const unsigned char*)"glGenVertexArrays");
    glGenBuffers_FA = (PFNGLGENBUFFERSPROC)glXGetProcAddress((const unsigned char*)"glGenBuffers");
    glGenVertexArrays_FA = (PFNGLGENVERTEXARRAYSPROC)glXGetProcAddress((const unsigned char*)"glGenVertexArrays");
    glBindBuffer_FA = (PFNGLBINDBUFFERPROC)glXGetProcAddress((const unsigned char*)"glBindBuffer");
    glBufferData_FA = (PFNGLBUFFERDATAPROC)glXGetProcAddress((const unsigned char*)"glBufferData");
    glVertexAttribPointer_FA = (PFNGLVERTEXATTRIBPOINTERPROC)glXGetProcAddress((const unsigned char*)"glVertexAttribPointer");
    glEnableVertexAttribArray_FA = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glXGetProcAddress((const unsigned char*)"glEnableVertexAttribArray");
    glUseProgram_FA = (PFNGLUSEPROGRAMPROC)glXGetProcAddress((const unsigned char*)"glUseProgram");
    glBindVertexArray_FA = (PFNGLBINDVERTEXARRAYPROC)glXGetProcAddress((const unsigned char*)"glBindVertexArray");
    glDeleteBuffers_FA = (PFNGLDELETEBUFFERSPROC)glXGetProcAddress((const unsigned char*)"glDeleteBuffers_FA");
    glDeleteVertexArrays_FA = (PFNGLDELETEVERTEXARRAYSPROC)glXGetProcAddress((const unsigned char*)"glDeleteVertexArrays_FA");
    
    //NOTE(Stanisz13): V_SYNC
    glXSwapIntervalMESA_FA = (PFNGLXSWAPINTERVALMESAPROC)glXGetProcAddress((const unsigned char*)"glXSwapIntervalMESA");
    glXSwapIntervalEXT_FA = (PFNGLXSWAPINTERVALEXTPROC)glXGetProcAddress((const unsigned char*)"glXSwapIntervalEXT_FA");
    glXSwapIntervalSGI_FA = (PFNGLXSWAPINTERVALSGIPROC)glXGetProcAddress((const unsigned char*)"glXSwapIntervalSGI_FA");
}

unsigned RGBAtoUnsigned(const unsigned char r, const unsigned char g,
                        const unsigned char b, const unsigned char a)
{
    return (a << 24) | (b << 16) | (g << 8) | r;    
}

void createTextureForDrawingBuffer(ContextData* cdata, PixelBufferData* pdata)
{    
    glGenTextures(1, &pdata->texture);
    glBindTexture(GL_TEXTURE_2D, pdata->texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 

    float vertices[] = {
        // positions       // texture coords
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // top right
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f  // top left 
    };

    unsigned indices[] = {  
        0, 1, 3, 2
    };

    unsigned int VBO, EBO;
    glGenVertexArrays_FA(1, &pdata->VAO);
    glGenBuffers_FA(1, &VBO);
    glGenBuffers_FA(1, &EBO);

    glBindVertexArray_FA(pdata->VAO);

    glBindBuffer_FA(GL_ARRAY_BUFFER, VBO);
    glBufferData_FA(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer_FA(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData_FA(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer_FA(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray_FA(0);
    // texture coord attribute
    glVertexAttribPointer_FA(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray_FA(1);
}

void drawTextureWithBufferData(ContextData* cdata, PixelBufferData* pdata)
{
    glBindTexture(GL_TEXTURE_2D, pdata->texture);
        
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cdata->windowWidth, cdata->windowHeight,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, pdata->pixels);
        
    glUseProgram_FA(pdata->basicProgram);
    glBindVertexArray_FA(pdata->VAO);
    glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, 0);
}

void freePixelData(PixelBufferData* pdata)
{
    free(pdata->pixels);
}

float lerp(const float v0, const float v1, const float t)
{
    return (1 - t) * v0 + t * v1;
}

Color lerpColor(const Color* a, const Color* b, const float t)
{
    Color res;
    res.r = (unsigned char)lerp(a->r, b->r, t);
    res.g = (unsigned char)lerp(a->g, b->g, t);
    res.b = (unsigned char)lerp(a->b, b->b, t);
    res.a = (unsigned char)lerp(a->a, b->a, t);

    return res;     
}

unsigned ColorToUnsigned(const Color* c)
{
    return RGBAtoUnsigned(c->r, c->g, c->b, c->a);
}

Color RGBAtoColor(const unsigned char r, const unsigned char g,
                  const unsigned char b, const unsigned char a)
{
    Color res;
    res.r = r;
    res.g = g;
    res.b = b;
    res.a = a;
    
    return res;
}

void configurePingpongBuffer(ContextData* cdata, PingpongBuffer* pbuf)
{
    glGenFramebuffers_FA(2, pbuf->fbo);
    glGenTextures(2, pbuf->texture);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer_FA(GL_FRAMEBUFFER, pbuf->fbo[i]);
        glBindTexture(GL_TEXTURE_2D, pbuf->texture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F,
                     cdata->windowWidth, cdata->windowHeight,
                     0, GL_RED, GL_FLOAT, 0);
          
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D_FA(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, pbuf->texture[i], 0);
    }        

}

void configureScreenQuadWithEBO(ScreenQuadWithEBO* squad)
{
    float screenQuadVerts[] =
        {
            1.0f,  1.0f,
            -1.0f, 1.0f,
            -1.0f, -1.0f,
            1.0f, -1.0f,
        };

    unsigned indices[] =
        {
            0, 1, 3, 2
        };

    glGenVertexArrays_FA(1, &squad->VAO);
    glGenBuffers_FA(1, &squad->VBO);
    glGenBuffers_FA(1, &squad->EBO);
    
    glBindVertexArray_FA(squad->VAO);

    glBindBuffer_FA(GL_ARRAY_BUFFER, squad->VBO);
    glBufferData_FA(GL_ARRAY_BUFFER, sizeof(screenQuadVerts), &screenQuadVerts, GL_STATIC_DRAW);

    glBindBuffer_FA(GL_ELEMENT_ARRAY_BUFFER, squad->EBO);
    glBufferData_FA(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer_FA(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray_FA(0);
}

void configureScreenQuad(ScreenQuad* squad)
{
    float screenQuadVerts[] =
        {
            1.0f,  1.0f,
            -1.0f, 1.0f,
            -1.0f, -1.0f,
            -1.0f, -1.0f,
            1.0f, -1.0f,
            1.0f, 1.0f
        };

    glGenVertexArrays_FA(1, &squad->VAO);
    glGenBuffers_FA(1, &squad->VBO);
    
    glBindVertexArray_FA(squad->VAO);

    glBindBuffer_FA(GL_ARRAY_BUFFER, squad->VBO);
    glBufferData_FA(GL_ARRAY_BUFFER, sizeof(screenQuadVerts), &screenQuadVerts, GL_STATIC_DRAW);

    glVertexAttribPointer_FA(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray_FA(0);
    
}


void freeScreenQuad(ScreenQuad* squad)
{
    glDeleteBuffers_FA(1, &squad->VBO);
    glDeleteVertexArrays_FA(1, &squad->VAO);
}

void freeScreenQuadWithEBO(ScreenQuadWithEBO* squad)
{
    glDeleteBuffers_FA(1, &squad->VBO);
    glDeleteVertexArrays_FA(1, &squad->VAO);
    glDeleteBuffers_FA(1, &squad->EBO);
}

void loadEntireFile(const char* path, unsigned char** source)
{
    FILE* file = fopen(path, "r");
    
    fseek(file, 0, SEEK_END);
    unsigned fileSize = ftell(file); 
    fseek(file, 0, SEEK_SET);

    *source = (unsigned char *)calloc(1, fileSize + 1);
    memset(*source, 0, fileSize + 1);

    if (fread(*source, fileSize, 1, file) != 1) {
        logError("Failed reading file!");
        logError(path);
        
        return;
    }
         
    fclose(file);
}

unsigned createShaderProgram(const char* pathToVS, const char* pathToFS)
{
    unsigned vertex, fragment;
    unsigned program;
    int success;
    char infoLog[512];

    unsigned char* vsCode;
    unsigned char* fsCode;
    loadEntireFile(pathToVS, &vsCode);
    loadEntireFile(pathToFS, &fsCode);
    
    vertex = glCreateShader_FA(GL_VERTEX_SHADER);
    fragment = glCreateShader_FA(GL_FRAGMENT_SHADER);
    glShaderSource_FA(vertex, 1, (const char**)&vsCode, NULL);
    glShaderSource_FA(fragment, 1, (const char**)&fsCode, NULL);
    
    glCompileShader_FA(vertex);
    glCompileShader_FA(fragment);
    
    glGetShaderiv_FA(vertex, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog_FA(vertex, 512, NULL, infoLog);
        printf("vertex shader error: %s\n", infoLog);
    }

    glGetShaderiv_FA(fragment, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog_FA(fragment, 512, NULL, infoLog);
        printf("fragment shader error: %s\n", infoLog);
    }
    
    program = glCreateProgram_FA();
    glAttachShader_FA(program, vertex);
    glAttachShader_FA(program, fragment);
    glLinkProgram_FA(program);

    glGetProgramiv_FA(program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog_FA(program, 512, NULL, infoLog);
        printf("%s\n", infoLog);     
    }

    glDeleteShader_FA(vertex);
    glDeleteShader_FA(fragment);

    free(vsCode);
    free(fsCode);
    
    return program;
}


void disableVSyncIfPossible(ContextData* cdata, UserVSyncData* udata)
{
    if (udata->mask == 0)
    {
        logWarning("No extension supporting v-sync control found! Will run without it...");
        return;
    }

    logWarning("V-SYNC DISABLED!");

    if (udata->mask & MESA_V_SYNC)
    {
        glXSwapIntervalMESA_FA(0);
        return;
    }

    if (udata->mask & EXT_V_SYNC)
    {
        glXSwapIntervalEXT_FA(cdata->display, glXGetCurrentDrawable(), 0);
        return;
    }

    if (udata->mask & SGI_V_SYNC)
    {
        glXSwapIntervalSGI_FA(0);
        return;
    }
}

void enableVSyncIfPossible(ContextData* cdata, UserVSyncData* udata)
{
    if (udata->mask == 0)
    {
        logWarning("No extension supporting v-sync control found! Will run without it...");
        return;
    }

    logWarning("V-SYNC ENABLED!");
    
    if (udata->mask & MESA_V_SYNC)
    {
        glXSwapIntervalMESA_FA(1);
        return;
    }

    if (udata->mask & EXT_V_SYNC)
    {
        glXSwapIntervalEXT_FA(cdata->display, glXGetCurrentDrawable(), 1);
        return;
    }

    if (udata->mask & SGI_V_SYNC)
    {
        glXSwapIntervalSGI_FA(1);
        return;
    }
}


void enableAdaptiveVSyncIfPossible(ContextData* cdata, UserVSyncData* udata)
{
    if (udata->mask == 0)
    {
        logWarning("No extension supporting v-sync control found! Will run without configuring it...");
        return;
    }

    logWarning("ADAPTIVE V-SYNC ENABLED!");
    
    if (udata->mask & MESA_V_SYNC)
    {
        glXSwapIntervalMESA_FA(-1);
        return;
    }

    if (udata->mask & EXT_V_SYNC)
    {
        glXSwapIntervalEXT_FA(cdata->display, glXGetCurrentDrawable(), -1);
        return;
    }

    if (udata->mask & SGI_V_SYNC)
    {
        glXSwapIntervalSGI_FA(-1);
        return;
    }
}
