#ifndef GLLOADER_H
#define GLLOADER_H

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include "glext.h"
#include "log.h"

typedef struct
{
    unsigned minimalGLXVersionMajor;
    unsigned minimalGLXVersionMinor;
    unsigned minimalGLVersionMajor;
    unsigned minimalGLVersionMinor;
    unsigned windowWidth;
    unsigned windowHeight;
    char* name;
    
    Display* display;
    Window window;
    Colormap cmap;
    GLXContext ctx;

    Atom deleteMessage;
    
} ContextData;

#if 0
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
#endif
PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;



void configureOpenGL(ContextData* cdata);

void clearConfigurationOfOpenGL(ContextData* cdata);

void loadFunctionPointers();

unsigned createBasicProgram();

unsigned RGBAtoUnsigned(const unsigned char r, const unsigned char g,
                        const unsigned char b, const unsigned char a);

#endif
