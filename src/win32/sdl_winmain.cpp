#ifdef _WIN32

/*
    SDL_main.c, placed in the public domain by Sam Lantinga  4/13/98

    Modified to write stdout/stderr to a message box at shutdown by Ripper  2007-12-27

    The WinMain function -- calls your program's main() function
*/

#include <exception>
#include <system_error>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/* Include the SDL main definition header */
#include "SDL.h"

#include "../CommandLine.h"
#include "../StdStringExtensions.h"

#ifdef main
#  undef main
#endif /* main */

/* This is where execution begins [windowed apps] */
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw)
{
	try
	{
		CommandLine::FeedFromWindows();
		return SDL_main(0, nullptr);
	}
	catch (const std::system_error& e)
	{
		std::string message(e.what());
		message += "(";
		message += e.code().value();
		message += ": ";
		message += e.code().message();
		message += ")";
		MessageBoxW(nullptr, UTF8ToWideChar(message.c_str()).c_str(), L"AutoWolf System Error", MB_OK | MB_ICONERROR);
		return 3;
	}
	catch (const std::exception& e)
	{
		MessageBoxW(nullptr, UTF8ToWideChar(e.what()).c_str(), L"AutoWolf Error", MB_OK | MB_ICONERROR);
		return 1;
	}
	catch (...)
	{
		MessageBox(nullptr, "An unknown error occurred! Please contact the author about this!", "AutoWolf Error", MB_OK | MB_ICONERROR);
		return 2;
	}
}

#endif  // _WIN32
