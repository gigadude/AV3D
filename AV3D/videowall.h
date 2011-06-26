#pragma once

#ifndef LMTZ_VIDEOWALL_H
#define LMTZ_VIDEOWALL_H

#define VIDEO_TEXTURE_RES 256

#include "glinclude.h"
#include "glshading.h"
#include "resource.h"
#include "glmesh.h"
#include "video.h"
#include "exception.h"

class VideoWall : public GLMesh
{
    GLShader *_vertexShader, *_fragmentShader;
    GLShaderProgram *_program;

    public:
    VideoWall(const char* filename, int width, int height)
    {   
        _width = width;
        _height = height;
        
        try
        {
            _video = new Video(filename, VIDEO_TEXTURE_RES, VIDEO_TEXTURE_RES);

            _vertexShader = new GLShader(ST_VERTEX, IDR_SHADER2);
            _fragmentShader = new GLShader(ST_FRAGMENT, IDR_SHADER1);
            _program = new GLShaderProgram();
            _program->Attach(_vertexShader);
            _program->Attach(_fragmentShader);
            _program->Link();

            _pixels = (uint8_t*) calloc(1, 3 * VIDEO_TEXTURE_RES * VIDEO_TEXTURE_RES );
            _texture = new GLTextureRGB(VIDEO_TEXTURE_RES, VIDEO_TEXTURE_RES, _pixels);

            _program->Use();
            _texture->Bind(GL_TEXTURE0);
            _program->BindTexture(GL_TEXTURE0, "_uTexture");
        }
        catch (Exception *e)
        {
            MessageBox(NULL, e->Message(), "ERROR", MB_OK);
            delete e;
            exit(-1);
        }

        
        GLVertex vertices[] = 
        {
            GLVertex(-1, -1, 0, 0, 1),
            GLVertex(-1,  1, 0, 0, 0),
            GLVertex( 1, -1, 0, 1, 1),
            GLVertex( 1,  1, 0, 1, 0)
        };

        short indices[] = {0, 1, 2, 3};
        

        SetVertexBuffer(vertices, 4);
        SetIndexBuffer(indices, 4);
        Update();

        _video->Start();
    }

    void Draw()
    {
        if (_video->NextFrame() == 1)
        {
            memcpy(_pixels, _video->PixelBuffer(), 3 * VIDEO_TEXTURE_RES * VIDEO_TEXTURE_RES);
            _texture->Update();
        }
        
        _program->Use();
        _texture->Bind(GL_TEXTURE0);
        _program->BindTexture(GL_TEXTURE0, "_uTexture");
        
        GLMesh::Draw();
    }


private:
    int _width, _height;
    Video* _video;
    uint8_t* _pixels;
    GLTextureRGB* _texture;
};

#endif