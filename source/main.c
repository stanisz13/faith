#include "glLoader.h"

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092

int isRunning = 1;

int main(int argc, char* argv[])
{
    ContextData contextData;
    contextData.minimalGLXVersionMajor = 1;
    contextData.minimalGLXVersionMinor = 3;
    contextData.minimalGLVersionMajor = 3;
    contextData.minimalGLVersionMinor = 0;
    contextData.windowWidth = 1600;
    contextData.windowHeight = 900;
    contextData.name = "Faith";
    
    configureOpenGL(&contextData);

    while(1)
    {
        glClearColor(0, 0.5, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        glXSwapBuffers (contextData.display, contextData.window);
    }

    clearConfigurationOfOpenGL(&contextData);
    
    return 0;
}
