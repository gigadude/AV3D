#pragma once

/* videowall.h
 * ----------- 
 * Author: E. Pronk
 * Descr.: VideoWall model, basic carrousel with funny mirror effect
 */

#ifndef LMTZ_VIDEOWALL_H
#define LMTZ_VIDEOWALL_H

#ifndef VIDEO_TEXTURE_RES_X 
#define VIDEO_TEXTURE_RES_X 256
#endif

#ifndef VIDEO_TEXTURE_RES_Y 
#define VIDEO_TEXTURE_RES_Y 256
#endif

#ifndef VIDEOWALL_NR_SIDES 
#define VIDEOWALL_NR_SIDES 7
#endif

#ifndef VIDEOWALL_SIDE_OFFSET 
#define VIDEOWALL_SIDE_OFFSET 3
#endif

#include <time.h>

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
    VideoWall(const char* filename);

    void Draw();    
    void Start();


private:
    int _width, _height;
    Video* _video;
    uint8_t* _pixels;
    GLTextureRGB* _texture;
};

#endif