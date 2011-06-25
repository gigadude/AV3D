#pragma once

#ifndef LMTZ_VIDEOWALL_H
#define LMTZ_VIDEOWALL_H

#include "glinclude.h"

class VideoWall
{
    public:
    VideoWall(int width, int height)
    {
        _width = width;
        _height = height;

        InitializeShader();
    }

    private:
    void CompileShader(GLuint shader, const char* source)
    {
        GLint status;

        glShaderSource(shader, 1, (const GLchar**)&source, NULL);
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE)
        {
            GLint length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            if (length > 0)
            {
                char* buffer = (char*)malloc(length);
                glGetShaderInfoLog(shader, length, &length, buffer);
                MessageBox(NULL, buffer, "ERROR", MB_OK);
                free(buffer);
                exit(-1);
            }
        }
    }

    void InitializeShader()
    {
        GLint status;

        _vertShader = glCreateShader(GL_VERTEX_SHADER);
        _fragShader = glCreateShader(GL_FRAGMENT_SHADER);

        HRSRC   vertRes = FindResource(NULL, MAKEINTRESOURCE(IDR_SHADER2), "Shader");
        HGLOBAL vertPtr = LoadResource(NULL, vertRes);
        LPVOID  vertSrc = LockResource(vertPtr);
        
        HRSRC   fragRes = FindResource(NULL, MAKEINTRESOURCE(IDR_SHADER1), "Shader");
        HGLOBAL fragPtr = LoadResource(NULL, fragRes);
        LPVOID  fragSrc = LockResource(fragPtr);
        
        CompileShader(_vertShader, (char*) vertSrc);
        CompileShader(_fragShader, (char*) fragSrc);
        
        _shaderProgram = glCreateProgram();
        glAttachShader(_vertShader, _shaderProgram);
        glAttachShader(_fragShader, _shaderProgram);
        glLinkProgram(_shaderProgram);

        glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &status);
        if (status == GL_FALSE) throw 1;
    }

    private:
    int _width, _height;
    GLuint _vertShader, _fragShader, _shaderProgram;
};

#endif