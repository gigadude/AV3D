#include "videowall.h"

VideoWall::VideoWall(const char* filename)
{
    _video = new Video(filename, VIDEO_TEXTURE_RES_X, VIDEO_TEXTURE_RES_Y);
    
    _vertexShader = new GLShader(ST_VERTEX, IDR_SHADER2);
    _fragmentShader = new GLShader(ST_FRAGMENT, IDR_SHADER1);
    _program = new GLShaderProgram();
    _program->Attach(_vertexShader);
    _program->Attach(_fragmentShader);
    _program->Link();

    _pixels = (uint8_t*) calloc(1, 3 * VIDEO_TEXTURE_RES_X * VIDEO_TEXTURE_RES_Y);
    _texture = new GLTextureRGB(VIDEO_TEXTURE_RES_X, VIDEO_TEXTURE_RES_Y, _pixels);
    
    _program->Use();
    _texture->Bind(GL_TEXTURE0);
    _program->BindTexture(GL_TEXTURE0, "_uTexture");

    _aspectRatio = (float)_video->Width() / (float)_video->Height();

    short indices[] = {0, 1, 2, 3};
    GLVertex vertices[] = 
    {
        GLVertex(-_aspectRatio , -1, 0, 0, 1),
        GLVertex(-_aspectRatio,  1, 0, 0, 0),
        GLVertex( _aspectRatio, -1, 0, 1, 1),
        GLVertex( _aspectRatio,  1, 0, 1, 0)
    };

    SetVertexBuffer(vertices, 4);
    SetIndexBuffer(indices, 4);
	CalculateNormals();
    Update();
}

void VideoWall::Draw()
{
    int status = _video->NextFrame(_pixels);
    if (status == 1)
    {
        _texture->Update();
    }
    
    int iOpacity = _program->GetUniformIndex("_uOpacity");
    int iYOpacity = _program->GetUniformIndex("_uYOpacity");
    int iTime = _program->GetUniformIndex("_uTime");
    
    _program->SetUniform(iTime, clock() / (CLOCKS_PER_SEC * 5.0f));

    for (int i = 0; i < VIDEOWALL_NR_SIDES; i++)
    {
        float angle = 10.0f * clock()/CLOCKS_PER_SEC;
        angle += i * 360.0f / VIDEOWALL_NR_SIDES;

        glPushMatrix();
        glRotatef(angle, 0, 1, 0);
        glTranslatef(0, 0, VIDEOWALL_SIDE_OFFSET * _aspectRatio);
			
        _program->SetUniform(iOpacity, 1.0f);
        _program->SetUniform(iYOpacity, 0.0f);
        GLMesh::Draw();

        glTranslatef(0.0f, -2.1f, 0.0f);
        glScalef(1, -1, 1);

        _program->SetUniform(iOpacity, 0.0f);
        _program->SetUniform(iYOpacity, 0.5f);
        GLMesh::Draw();

        glPopMatrix();
    }
}

void VideoWall::Start()
{
    _video->Start();
}
