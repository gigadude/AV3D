#pragma once

#ifndef LMTZ_VIDEOWALL_H
#define LMTZ_VIDEOWALL_H

#include "glinclude.h"
#include "glshading.h"
#include "exception.h"

class VideoWall
{
    GLShader *_vertexShader, *_fragmentShader;
    GLShaderProgram *_program;

    public:
    VideoWall(int width, int height)
    {
        _width = width;
        _height = height;

        try
        {
            _vertexShader = new GLShader(ST_VERTEX, IDR_SHADER2);
            _fragmentShader = new GLShader(ST_FRAGMENT, IDR_SHADER1);
            _program = new GLShaderProgram();
            _program->Attach(_vertexShader);
            _program->Attach(_fragmentShader);
            _program->Link();
        }
        catch (ShaderException *e)
        {
            MessageBox(NULL, e->Message(), "ERROR", MB_OK);
            delete e;
            exit(-1);
        }
        InitializeBuffers();
    }

    public:
    void Prepare()
    {
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
    }

    public:
    void Draw()
    {
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    private:
    void InitializeBuffers()
    {
        glGenBuffers(1, &_vbo);
        glGenBuffers(1, &_ibo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
    }

private:
    int _width, _height;
    GLuint _vbo, _ibo;
};

#endif