#ifndef _GLWINDOW_H
#define _GLWINDOW_H

#include <windows.h>
#include <ctime>
#include "GameEngine.h"

class Example; //Declare our Example class

class GLWindow 
{
public:
    GLWindow(HINSTANCE hInstance); //default constructor

    bool create(int width, int height, int bpp, bool fullscreen);
    void destroy();
    void processEvents();
    void attachEngine(GameEngine *engine);
    
    bool isRunning(); //Is the window running?

    void swapBuffers() { SwapBuffers(m_hdc); }

    static LRESULT CALLBACK StaticWndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

    float getElapsedSeconds();
private:
    GameEngine* m_engine; //A link to the game engine
    bool m_isRunning; //Is the window still running?
    bool m_isFullscreen; 

    HWND m_hwnd; //Window handle
    HGLRC m_hglrc; //Rendering context
    HDC m_hdc; //Device context
    RECT m_windowRect; //Window bounds
    HINSTANCE m_hinstance; //Application instance
    WNDCLASSEX m_windowClass;

    void setupPixelFormat(void);
    GameEngine* getAttachedEngine() { return m_engine; }

    float m_lastTime;
};

#endif