#include "graphics.h"

int isRunning = 1;

int main(int argc, char* argv[])
{
    ContextData contextData;
    contextData.minimalGLXVersionMajor = 1;
    contextData.minimalGLXVersionMinor = 3;
    contextData.minimalGLVersionMajor = 3;
    contextData.minimalGLVersionMinor = 3;
    contextData.windowWidth = 600;
    contextData.windowHeight = 600;
    contextData.name = "Faith";
    
    configureOpenGL(&contextData);
    loadFunctionPointers();

    unsigned pixelsSize = contextData.windowHeight * contextData.windowWidth * sizeof(unsigned);
    unsigned* pixels = (unsigned*)malloc(pixelsSize);

    unsigned* running = pixels;
    unsigned y, x;
    for (y = 0; y < contextData.windowHeight; ++y)
    {
        for (x = 0; x < contextData.windowWidth; ++x)
        {
            *running = 0x00000000;
            
            if (x*x + y*y < 300*300)
                *running = 0x00FF00FF;

            ++running;
        }
    }
    
    unsigned texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
  
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, contextData.windowWidth, contextData.windowHeight,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    
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

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    
    unsigned basicProgram = createBasicProgram();
    
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

        glBindTexture(GL_TEXTURE_2D, texture);

        glUseProgram(basicProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, 0);

        
        glXSwapBuffers(contextData.display, contextData.window);
    }

    clearConfigurationOfOpenGL(&contextData);

    free(pixels);
    
    return 0;
}
