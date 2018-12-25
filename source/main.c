#include "graphics.h"
#include <math.h>

typedef struct
{
    unsigned sand;
    unsigned flag;
    
} Tile;

typedef struct
{
    unsigned x, y;
} Pair;

#define MAX_WIDTH 2000
#define MAX_HEIGHT 2000

int isRunning = 1;

Tile map[MAX_WIDTH][MAX_HEIGHT];
unsigned colors[MAX_WIDTH * MAX_HEIGHT];
Pair prevProcess[MAX_WIDTH * MAX_HEIGHT];
Pair toProcess[MAX_WIDTH * MAX_HEIGHT];
unsigned prevProcessIdx;
unsigned toProcessIdx;

void update(const ContextData* cdata)
{
    unsigned y, x;
    toProcessIdx = 0;
        
    for (y = 0; y < prevProcessIdx; ++y)
    {
        Pair center = prevProcess[y];

        Tile* hit = &map[center.x][center.y];
        hit->sand -= 4;

        if (hit->sand > 3)
        {
            toProcess[toProcessIdx++] = center;        
        }
        else
        {
            map[center.x][center.y].flag = 0;
        }
        
        if (center.y > 0)
        {
            Tile* top = &map[center.x][center.y - 1];
            Pair cur;
            cur.x = center.x;
            cur.y = center.y - 1;
           
            if (++top->sand > 3)
            {
                if(top->flag == 0)
                {
                    toProcess[toProcessIdx++] = cur;

                    map[cur.x][cur.y].flag = 1;
                }
            }
            else
            {
                map[cur.x][cur.y].flag = 0;
            }
        }
        if (center.y < cdata->windowHeight - 1)
        {
            Tile* bot = &map[center.x][center.y + 1];
            Pair cur;
            cur.x = center.x;
            cur.y = center.y + 1;

            if (++bot->sand > 3)
            {
                if (bot->flag == 0)
                {
                    toProcess[toProcessIdx++] = cur;

                    map[cur.x][cur.y].flag = 1;
                }
            }
            else
            {
                map[cur.x][cur.y].flag = 0;
            }
        }
        if (center.x > 0)
        {
            Tile* left = &map[center.x - 1][center.y];
            Pair cur;
            cur.x = center.x - 1;
            cur.y = center.y;
             
            if (++left->sand > 3)
            {
                if (left->flag == 0)
                {
                    toProcess[toProcessIdx++] = cur;

                    map[cur.x][cur.y].flag = 1;

                }
            }
            else
            {
                map[cur.x][cur.y].flag = 0;
            }
        }
        if (center.x < cdata->windowWidth - 1)
        {
            Tile* right = &map[center.x + 1][center.y];
            Pair cur;
            cur.x = center.x + 1;
            cur.y = center.y;
             
            if (++right->sand > 3)
            {
                if (right->flag == 0)
                {
                    toProcess[toProcessIdx++] = cur;

                    map[cur.x][cur.y].flag = 1;

                }
            }
            else
            {
                map[cur.x][cur.y].flag = 0;
            }
        }
    }

    for (unsigned i = 0; i < toProcessIdx; ++i)
    {
        prevProcess[i] = toProcess[i];
    }

    prevProcessIdx = toProcessIdx;
}

void updateColors(ContextData* cdata)
{
    unsigned x, y;
    
    for (y = 0; y < cdata->windowHeight; ++y)
    {
        for (x = 0; x < cdata->windowWidth; ++x)
        {
            Tile* now = &map[x][y];
            switch(now->sand)
            {
                case 0:
                    colors[y * cdata->windowWidth + x] = RGBAtoUnsigned(20, 10, 30, 0);
                    break;
                case 1:
                    colors[y * cdata->windowWidth + x] = RGBAtoUnsigned(50, 20, 10, 0);
                    break;
                case 2:
                    colors[y * cdata->windowWidth + x] = RGBAtoUnsigned(100, 0, 180, 0);
                    break;
                case 3:
                    colors[y * cdata->windowWidth + x] = RGBAtoUnsigned(0, 200, 50, 0);
                    break;
                default:
                    colors[y * cdata->windowWidth + x] = RGBAtoUnsigned(255, 0, 0, 0);
                    break;
            }
        }
    }
}

void addSource(const unsigned posX, const unsigned posY, const unsigned sand)
{
    Pair p;
    p.x = posX;
    p.y = posY;
    map[posX][posY].sand = sand;
    map[posX][posY].flag = 1;
    prevProcess[prevProcessIdx++] = p;    
}

void addEquilateralSources(const unsigned count, const unsigned radius,
                           const unsigned windowWidth, const unsigned windowHeight,
                           const unsigned sand)
{
    const float PI = 3.14159265358f;
    const float alpha = 360.0f / count;
    const float alphaInRad = (PI * alpha) / 180.0f;
    const float edge = sqrt(2) * radius * (1 - cos(alphaInRad));
    const float centerX = windowWidth / 2;
    const float centerY = windowHeight / 2;
    
    for (unsigned i = 0; i < count; ++i)
    {
        const float angle = 90.0f - alpha * i;
        const float angleInRad = (PI * angle) / 180.0f;
    
        const float dx = sqrt(2) * radius * (1 - cos(angleInRad));
        const float dy = sqrt(2) * radius * (1 - sin(angleInRad));

        const float x = centerX + dx;
        const float y = centerY + dy;

        addSource(x, y, sand);
    }
}

int main(int argc, char* argv[])
{
    ContextData contextData;
    contextData.minimalGLXVersionMajor = 1;
    contextData.minimalGLXVersionMinor = 3;
    contextData.minimalGLVersionMajor = 3;
    contextData.minimalGLVersionMinor = 3;
    contextData.windowWidth = 1600;
    contextData.windowHeight = 900;
    contextData.name = "Faith";
    
    configureOpenGL(&contextData);
    loadFunctionPointers();

    unsigned y, x;

#if 0
    addSource(contextData.windowWidth / 2, contextData.windowHeight / 2, 2000000);
    addSource(contextData.windowWidth * 0.60f, contextData.windowHeight / 2, 2000000);
#endif

    addEquilateralSources(10, 100, contextData.windowWidth, contextData.windowHeight, 2000000);
    
    unsigned texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
  
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, contextData.windowWidth, contextData.windowHeight,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, colors);
    
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

        unsigned sandUpdatesPerFrame = 1000;
        for (unsigned i = 0; i < sandUpdatesPerFrame; ++i)
        {
            update(&contextData);
        }

        updateColors(&contextData);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, contextData.windowWidth, contextData.windowHeight,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, colors);
        
        glBindTexture(GL_TEXTURE_2D, texture);
        glUseProgram(basicProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, 0);

        glXSwapBuffers(contextData.display, contextData.window);
//        sleep(1);
    }

    clearConfigurationOfOpenGL(&contextData);

    return 0;
}
