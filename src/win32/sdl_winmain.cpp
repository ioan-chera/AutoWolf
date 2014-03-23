#ifdef _WIN32

/*
    SDL_main.c, placed in the public domain by Sam Lantinga  4/13/98

    Modified to write stdout/stderr to a message box at shutdown by Ripper  2007-12-27

    The WinMain function -- calls your program's main() function
*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/* Include the SDL main definition header */
#include "SDL.h"

#ifdef main
#  undef main
#endif /* main */

/* This is where execution begins [windowed apps] */
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw)
{
	return SDL_main(0, nullptr);
}

#endif  // _WIN32
