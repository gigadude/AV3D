#include <windows.h>
#include <stdio.h>

#include "avinclude.h"
#include "glinclude.h"
#include "glwindow.h"
#include "videowall.h"

class AV3DWindow : public GLWindow
{
    public:
    AV3DWindow(HINSTANCE hInstance, const char* moviePath) : GLWindow(hInstance, "AV3D", 800, 600)
    {
        glEnable(GL_TEXTURE_2D);
        glShadeModel(GL_SMOOTH);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

        _videoWall = new VideoWall(moviePath, 16, 9);
    }

    void OnRender()
    {   
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _videoWall->Draw();
        Sleep(30);
    }

    private:
    VideoWall* _videoWall;
 
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    GLWindow* window = new AV3DWindow(hInstance, "D:\\phunk.avi");
    window->Show();

    return window->MainLoop();
}