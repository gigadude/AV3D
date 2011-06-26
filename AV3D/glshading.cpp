#include "glshading.h"

#pragma region GLShader

GLShader::GLShader(ShaderType type, const char* sourceCode)
{   
    _name = glCreateShader((GLenum)type);
    SetSource(sourceCode);    
}

GLShader::GLShader(ShaderType type, int resourceId)
{
    _name = glCreateShader((GLenum)type);

    HRSRC resource = FindResource(NULL, MAKEINTRESOURCE(resourceId), "Shader");
    HGLOBAL global = LoadResource(NULL, resource);
    char* sourceCode = (char*) LockResource(global);
    SetSource(sourceCode);
}

void GLShader::SetSource(const char* sourceCode)
{
    int status = GL_FALSE;
    
    glShaderSource(_name, 1, &sourceCode, 0);
    glCompileShader(_name);
    glGetShaderiv(_name, GL_COMPILE_STATUS, &status);

    if (status != GL_TRUE)
    {
        int length;
        glGetShaderiv(_name, GL_INFO_LOG_LENGTH, &length);
        if (length > 0)
        {
            char* error = (char*) malloc(length);
            glGetShaderInfoLog(_name, length, 0, error);
            throw new ShaderCompilationException(error);
        }
    }
}

GLShader::~GLShader()
{
    glDeleteShader(_name);
}

#pragma endregion

#pragma region GLShaderProgram

GLShaderProgram::GLShaderProgram()
{
    _name = glCreateProgram();
    _isLinked = false;
}

GLShaderProgram::~GLShaderProgram()
{
    glDeleteProgram(_name);
}

void GLShaderProgram::Attach(GLShader* shader)
{
    glAttachShader(_name, shader->_name);
    _isLinked = false;
}

void GLShaderProgram::Detach(GLShader* shader)
{
    glDetachShader(_name, shader->_name);
}

void GLShaderProgram::BindAttribute(unsigned int index, const char* name)
{
    glBindAttribLocation(_name, index, name);
}

void GLShaderProgram::BindTexture(GLenum texture, const char* name)
{
    int uniform = glGetUniformLocation(_name, name);
    glUniform1i(uniform, texture - GL_TEXTURE0);
}

int GLShaderProgram::GetUniformIndex(const char *name)
{
    return glGetUniformLocation(_name, name);
}

void GLShaderProgram::Link()
{
    GLVertex::Declare();
    GLint status;
    for (int i=0; i<VA_MAX; i++)
	{
		const char* name = GLVertex::GetAttributeName((VertexAttribute)i);
		glBindAttribLocation(_name, i, name);
	}
    glLinkProgram(_name);
    glGetProgramiv(_name, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) throw new ShaderLinkException();
    _isLinked = true;
}

void GLShaderProgram::Use()
{
    if (!_isLinked) Link();
    glUseProgram(_name);
}

#pragma endregion
