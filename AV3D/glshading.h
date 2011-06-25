#pragma once

#ifndef LMTZ_SHADING_H
#define LMTZ_SHADING_H

#include <windows.h>
#include "glinclude.h"
#include "exception.h"

GENERATE_EXCEPTION(ShaderException, Exception);
GENERATE_EXCEPTION(ShaderCompilationException, ShaderException);
GENERATE_EXCEPTION(ShaderLinkException, ShaderException);

enum ShaderType
{
    ST_VERTEX = GL_VERTEX_SHADER,
    ST_FRAGMENT = GL_FRAGMENT_SHADER
};

class GLShader
{
    public:
    GLShader(ShaderType type, const char* sourceCode);
    GLShader(ShaderType type, int resourceId);
    ~GLShader();

    protected:
    void SetSource(const char* sourceCode);

    private:
    GLuint _name;

    friend class GLShaderProgram;
};

class GLShaderProgram
{
    public:
    GLShaderProgram();
    ~GLShaderProgram();

    void Attach(GLShader* shader);
    void Detach(GLShader* shader);

    void BindAttribute(unsigned int index, const char* name);
    void BindTexture(GLenum texture, const char* name);
    int  GetUniformIndex(const char* name);

    void Link();
    void Use();

    private:
    bool _isLinked;
    GLuint _name;
};

#endif