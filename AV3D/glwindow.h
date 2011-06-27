#pragma once

#ifndef LMTZ_GLWINDOW_H
#define LMTZ_GLWINDOW_H

/* glwindow.h
 * ----------
 * Author: E. Pronk
 * Descr.: Base class for an OpenGL window
 */

#include <windows.h>
#include <windowsx.h>
#include "glinclude.h"

#ifndef GLWINDOW_STYLE
#define GLWINDOW_STYLE WS_OVERLAPPEDWINDOW
#endif

#ifndef GLWINDOW_STYLE_EX
#define GLWINDOW_STYLE_EX 0
#endif

#ifndef GLWINDOW_CLASSNAME
#define GLWINDOW_CLASSNAME "LMTZ_GLWINDOW"
#endif

class GLWindow
{
    public:
    GLWindow(HINSTANCE hInstance, const char* title, int width, int height);
    ~GLWindow();
    
    void Show();
    int MainLoop();

    protected:
    virtual void OnLoad() {}
    virtual void OnRender();
    virtual void OnResize(int width, int height);
    virtual void OnKeyDown(int key) {};
    virtual void OnMouseDown(int x, int y) {};
    virtual void OnMouseMove(int x, int y) {};
    virtual void OnMouseUp(int x, int y) {};

    private:
    HWND        _hwnd;
    HGLRC       _glrc;
    static LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
};

#endif