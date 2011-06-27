#pragma once

/* glshading.h
 * ----------- 
 * Author: E. Pronk
 * Descr.: Everything needed for shaders
 */

#ifndef LMTZ_SHADING_H
#define LMTZ_SHADING_H

#include <windows.h>
#include "glinclude.h"
#include "gltexture.h"
#include "glvertex.h"
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
    void SetSource(const char* sourceCode, size_t size);

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

    void SetUniform(int index, const GLTextureRGB* texture)         const { glUniform1i(index, texture->_name); }

	void SetUniform(int index, float value)							const { glUniform1f(index, value); }
	void SetUniform(int index, float x, float y)					const { glUniform2f(index, x, y); }
	void SetUniform(int index, float x, float y, float z)			const { glUniform3f(index, x, y, z); }
	void SetUniform(int index, float x, float y, float z, float w)  const { glUniform4f(index, x, y, z, w); }

	void SetUniform(int index, int value)							const { glUniform1i(index, value); }
	void SetUniform(int index, int x, int y)						const { glUniform2i(index, x, y); }
	void SetUniform(int index, int x, int y, int z)					const { glUniform3i(index, x, y, z); }
	void SetUniform(int index, int x, int y, int z, int w)			const { glUniform4i(index, x, y, z, w); }
	
	void SetUniformMatrix(int index, const float* matrix)			const { glUniformMatrix4fv(index, 1, GL_FALSE, matrix); }

    void Link();
    void Use();

    private:
    bool _isLinked;
    GLuint _name;
};

#endif