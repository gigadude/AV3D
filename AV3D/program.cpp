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
        _distance = VIDEOWALL_SIDE_OFFSET + 8;
		_camera = Vector3D(0, 0,  _distance);

        _isMouseDown = false;
        glEnable(GL_TEXTURE_2D);
        glShadeModel(GL_SMOOTH);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glEnable(GL_POLYGON_SMOOTH);

        _videoWall = new VideoWall(moviePath);
        _videoWall->Start();
    }

	void OnMouseDown(int x, int y)
	{
		_refX = x;
		_refY = y;
		_isMouseDown = true;
	}

	void OnMouseMove(int x, int y)
	{   
        if (_isMouseDown)
		{
            float angle = atan2(_camera.Z(),_camera.X()) + (x - _refX)/200.0f;
			_distance +=(y - _refY)/100.0f;

            _camera.X() = cos(angle) * _distance;
			_camera.Z() = sin(angle) * _distance;
			_refX = x;
			_refY = y;
		}
	}

	void OnMouseUp(int x, int y)
	{
		_isMouseDown = false;
	}

    void OnRender()
    {   
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		gluLookAt(_camera.X(), _camera.Y(), _camera.Z(), 
				  0, 0,  0,
				  0, 1,  0);
		
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		_videoWall->Draw();
       glPopMatrix();
       glFlush();
       glFinish();
    }

    private:
    VideoWall* _videoWall;
    Vector3D _camera;
	
    bool _isMouseDown;
	int _refX, _refY;
    
    float _distance;
    float _initialRadians;
 
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    GLWindow* window = new AV3DWindow(hInstance, "./phunk.avi");
    window->Show();

    return window->MainLoop();
}