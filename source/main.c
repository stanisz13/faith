#include "graphics.h"

int isRunning = 1;

typedef struct
{
    unsigned a, b;
} pair;

void update(unsigned* sand, unsigned* pixels, const ContextData* cdata)
{
    unsigned y, x;
    pair* toProcess = (pair*)malloc(sizeof(pair) * cdata->windowWidth * cdata->windowHeight);
    unsigned processIdx = 0;

    unsigned* running = sand;
    for (y = 0; y < cdata->windowHeight; ++y)
    {
        for (x = 0; x < cdata->windowWidth; ++x)
        {
            if (*running > 3)
            {
                pair cur;
                cur.a = y;
                cur.b = x;
                toProcess[processIdx++] = cur;
            }
            //  printf("%d ", *running);
            ++running;
        }
        //      printf("\n");
    }
//    printf("\n\n");

    for (y = 0; y < processIdx; ++y)
    {
        pair cur = toProcess[y];

        unsigned* hit = sand + cur.a * cdata->windowWidth + cur.b;
        *hit -= 4;
        
        if (cur.a > 0)
        {
            unsigned* top = hit - cdata->windowWidth;
            ++*top;
        }
        if (cur.a < cdata->windowHeight - 1)
        {
            unsigned* bot = hit + cdata->windowHeight;
            ++*bot;
        }
        if (cur.b > 0)
        {
            unsigned* left = hit - 1;
            ++*left;
        }
        if (cur.b < cdata->windowWidth - 1)
        {
            unsigned* right = hit + 1;
            ++*right;
        }
    }

    free(toProcess);

    running = sand;
    unsigned* running2 = pixels;
    for (y = 0; y < cdata->windowHeight; ++y)
    {
        for (x = 0; x < cdata->windowWidth; ++x)
        {
            switch(*running)
            {
                case 0:
                    *running2 = 0x55542325;
                    break;
                case 1:
                    *running2 = 0x0000FF00;
                    break;
                case 2:
                    *running2 = 0x000000FF;
                    break;
                case 3:
                    *running2 = 0x00FF0000;
                    break;
                default:
                    *running2 = 0x00000000;
                    break;
                    
            }
            
            ++running;
            ++running2;
        }
    }

}

int main(int argc, char* argv[])
{
    ContextData contextData;
    contextData.minimalGLXVersionMajor = 1;
    contextData.minimalGLXVersionMinor = 3;
    contextData.minimalGLVersionMajor = 3;
    contextData.minimalGLVersionMinor = 3;
    contextData.windowWidth = 501;
    contextData.windowHeight = 501;
    contextData.name = "Faith";
    
    configureOpenGL(&contextData);
    loadFunctionPointers();

    unsigned pixelsSize = contextData.windowHeight * contextData.windowWidth * sizeof(unsigned);
    unsigned* pixels = (unsigned*)malloc(pixelsSize);
    unsigned* sand = (unsigned*)malloc(pixelsSize);
    
    unsigned* running = sand;
    unsigned y, x;

    for (y = 0; y < contextData.windowHeight; ++y)
    {
        for (x = 0; x < contextData.windowWidth; ++x)
        {
            *running = 0;
            
            if (x == contextData.windowWidth / 2 && y == contextData.windowHeight/2)
            {
                *running = 4000000;
            }
            
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

        for (unsigned i = 0; i < 100; ++i)
        update(sand, pixels, &contextData);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, contextData.windowWidth, contextData.windowHeight,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        
        glBindTexture(GL_TEXTURE_2D, texture);
        glUseProgram(basicProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, 0);

        glXSwapBuffers(contextData.display, contextData.window);
        //    sleep(1);
    }

    clearConfigurationOfOpenGL(&contextData);

    free(pixels);
    free(sand);

    return 0;
}
