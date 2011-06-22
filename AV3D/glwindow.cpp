#include "glwindow.h"
#include <stdio.h>

GLWindow::GLWindow(HINSTANCE hInstance, const char* title, int width, int height)
{
    RECT windowRect;
    HDC hdc;

    static PIXELFORMATDESCRIPTOR ls_pfd = { 
        sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW, PFD_TYPE_RGBA, 
        32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 8, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 };

    static WNDCLASSEX ls_wc = { 
        sizeof(WNDCLASSEX), CS_OWNDC | CS_HREDRAW | CS_VREDRAW, GLWindow::WindowProc,
        0, 0, hInstance, NULL, LoadCursor(NULL, IDC_ARROW), NULL, NULL, GLWINDOW_CLASSNAME, NULL };
    
    RegisterClassEx(&ls_wc);
    SetRect(&windowRect, 0, 0, width, height);
    AdjustWindowRectEx(&windowRect, GLWINDOW_STYLE, FALSE, GLWINDOW_STYLE_EX);

    // Create Window
    _hwnd = CreateWindowEx(GLWINDOW_STYLE_EX, GLWINDOW_CLASSNAME, title, GLWINDOW_STYLE, 
        CW_USEDEFAULT, CW_USEDEFAULT, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
        NULL, NULL, hInstance, this);

    // Create GL Context
    hdc = GetDC(_hwnd);
    SetPixelFormat(hdc, ChoosePixelFormat(hdc, &ls_pfd), &ls_pfd);
    _glrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, _glrc);
    ReleaseDC(_hwnd, hdc);

    // Initialize GLEW
    GLenum error = glewInit();
    if (error != GLEW_OK) { return; }

    glClearColor(0, 0, 0, 0xFF);
}

GLWindow::~GLWindow()
{
    HDC hdc = GetDC(_hwnd);
    wglMakeCurrent(hdc, NULL);
    wglDeleteContext(_glrc);
    ReleaseDC(_hwnd, hdc);
}

void GLWindow::Show()
{
    ShowWindow(_hwnd, SW_SHOWNORMAL);
    UpdateWindow(_hwnd);
}

void GLWindow::OnRender()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLWindow::OnResize(int width, int height)
{
    glViewport(0, 0, width, height);
}

LRESULT CALLBACK GLWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    GLWindow* instance = (GLWindow*) GetWindowLongPtr(hwnd, GWL_USERDATA);

    switch (msg)
    {
        case WM_CREATE:
        {
			CREATESTRUCT* create = (CREATESTRUCT*) lParam;
			instance = (GLWindow*) create->lpCreateParams;
			SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR) instance);
			return 0;
		}
        case WM_SIZE:
		    instance->OnResize(LOWORD(lParam), HIWORD(lParam));
			return 0;
    
        case WM_CLOSE:
            DestroyWindow(hwnd);
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_KEYDOWN:
            instance->OnKeyDown(wParam);
            return 0;

        case WM_RBUTTONDOWN:
        case WM_LBUTTONDOWN:
            SetCapture(hwnd);
            instance->OnMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return 0;

        case WM_RBUTTONUP:
        case WM_LBUTTONUP:
            ReleaseCapture();
            instance->OnMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return 0;

        case WM_MOUSEMOVE:
            instance->OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT:
            if (GetUpdateRect(hwnd, NULL, FALSE)) 
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                instance->OnRender();
                SwapBuffers(hdc);
                EndPaint(hwnd, &ps);
            }
            return 0;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

int GLWindow::MainLoop()
{
    MSG msg;
    while (true)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) return msg.lParam;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        InvalidateRect(_hwnd, NULL, FALSE);
    }
}