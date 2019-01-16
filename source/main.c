#include "graphics_FA.h"
#include <math.h>
#include <time.h>
#include "maths_FA.h"

int isRunning = 1;

int main(int argc, char* argv[])
{
    newLine();
    newLine();
    
    ContextData contextData;
    contextData.minimalGLXVersionMajor = 1;
    contextData.minimalGLXVersionMinor = 3;
    contextData.minimalGLVersionMajor = 3;
    contextData.minimalGLVersionMinor = 3;
    contextData.windowWidth = 1920;
    contextData.windowHeight = 1080;
    contextData.name = "Faith";

    UserVSyncData userVSyncData;

    configureOpenGL(&contextData, &userVSyncData);
    loadFunctionPointers();
    
#if USE_EBO_TO_DRAW_QUAD == 1
    ScreenQuadWithEBO squad;
    configureScreenQuadWithEBO(&squad);
#else
    ScreenQuad squad;
    configureScreenQuad(&squad);
#endif
        
    glDisable(GL_DEPTH_TEST);

#if 0
    disableVSyncIfPossible(&contextData, &userVSyncData);
#else
    enableVSyncIfPossible(&contextData, &userVSyncData);
#endif

    struct timespec prevTime;
    clock_gettime(CLOCK_MONOTONIC_RAW, &prevTime);
    float dt = 0.0f;
    float elapsed = 0.0f;
    float maxFrameTimeNoticed = 0.0f;
    
    while(1)
    {        
        XEvent event;

        while (XPending(contextData.display))
        {
            XNextEvent(contextData.display, &event);
            switch (event.type)
            {
                case ClientMessage:
                    if (event.xclient.data.l[0] == contextData.deleteMessage)
                        isRunning = 0;
                    break;
            }
        }

        if (isRunning == 0)
        {
            break;
        }

        glClearColor(0, 0.5, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        
#if USE_EBO_TO_DRAW_QUAD == 1
        glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, 0);
#else
        glDrawArrays(GL_TRIANGLES, 0, 6);
#endif
        
        glXSwapBuffers(contextData.display, contextData.window);

        struct timespec nowTime;
        clock_gettime(CLOCK_MONOTONIC_RAW, &nowTime);
        dt = (nowTime.tv_sec - prevTime.tv_sec) * 1000000
            + (nowTime.tv_nsec - prevTime.tv_nsec) / 1000;
        dt /= 1000.0f;
        
        prevTime = nowTime;

        elapsed += dt;
        if (dt - maxFrameTimeNoticed > EPSILON)
        {
            maxFrameTimeNoticed = dt;
        }

#if 1
        if (elapsed > 2.0f * 1000)
        {
            printf("Longest frame: %f [%f FPS]\n", maxFrameTimeNoticed, 1000.0f / maxFrameTimeNoticed);
            elapsed = 0.0f;
            maxFrameTimeNoticed = 0.0f;
        }
#else
        printf("dt = %f [%f FPS]\n", dt, 1.0f/dt);
#endif
    }

    freeContextData(&contextData);

#if USE_EBO_TO_DRAW_QUAD == 1
    freeScreenQuadWithEBO(&squad);
#else
    freeScreenQuad(&squad);
#endif

    return 0;
}
