#include "gltexture.h"

GLTextureRGB::GLTextureRGB(int width, int height, const void *pixels)
{
    GLenum error;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &_name);
    glBindTexture(GL_TEXTURE_2D, _name);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    _width = width;
    _height = height;
    _pixels = pixels;

    Update();
    error = glGetError();
    if (error != GL_NO_ERROR)
    {
        throw new TextureException("Texture Exception 0x(%08X)", error);
    }
}

GLTextureRGB::~GLTextureRGB()
{
    glDeleteTextures(1, &_name);
}

void GLTextureRGB::Update()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _name);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, _pixels);
}

void GLTextureRGB::Bind(GLenum texture)
{
    glActiveTexture(texture);
    glBindTexture(GL_TEXTURE_2D, _name);
}