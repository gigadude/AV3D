#pragma once

/* gltexture.h
 * ----------- 
 * Author: E. Pronk
 * Descr.: Defines a texture class using 24bpp RGB format
 */

#ifndef LMTZ_GLTEXTURE_H
#define LMTZ_GLTEXTURE_H

#include "glinclude.h"
#include "exception.h"

GENERATE_EXCEPTION(TextureException, Exception);

class GLTextureRGB
{
    public:
    GLTextureRGB(int width, int height, const void* pixels);
    ~GLTextureRGB();

    void Update();
    void Bind(GLenum texture);
    
    private:
    GLuint _name;
    int _width, _height;
    const void* _pixels;

    friend class GLShaderProgram;
};

#endif