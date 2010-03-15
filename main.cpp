/* FPSDemo
 * David Schneider - 01/10
 * main.cpp and glwindow class adapted from Beginning OpenGL Game Programming Second Edition by Benstead, Astle & Hawkins
 */

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include <windows.h>
#include "glwindow.h"
#include "GameEngine.h"

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR cmdLine,
                   int cmdShow)
{
    //Set our window settings
    const int windowWidth = 1024;
    const int windowHeight = 768;
    const int windowBPP = 16;
    const int windowFullscreen = false;

    //This is our window
    GLWindow programWindow(hInstance);

    //The example OpenGL code
    GameEngine engine;

    //Attach our example to our window
    programWindow.attachEngine(&engine);

    //Attempt to create the window
    if (!programWindow.create(windowWidth, windowHeight, windowBPP, windowFullscreen))
    {
        //If it fails
        MessageBox(NULL, "Unable to create the OpenGL Window", "An error occurred", MB_ICONERROR | MB_OK);
        programWindow.destroy(); //Reset the display and exit
        return 1;
    }

    if (!engine.init()) //Initialize our example
    {
        MessageBox(NULL, "Could not initialize the application", "An error occurred", MB_ICONERROR | MB_OK);
        programWindow.destroy(); //Reset the display and exit
        return 1;
    }

    //This is the mainloop, we render frames until isRunning returns false
    while(programWindow.isRunning())
    {
        programWindow.processEvents(); //Process any window events

        //We get the time that passed since the last frame
        float elapsedTime = programWindow.getElapsedSeconds();

        engine.prepare(elapsedTime); //Do any pre-rendering logic
        engine.render(); //Render the scene

        programWindow.swapBuffers();
    }

    engine.shutdown(); //Free any resources
    programWindow.destroy(); //Destroy the program window

    return 0; //Return success
}
