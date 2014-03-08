#ifdef _WIN32

/*
    SDL_main.c, placed in the public domain by Sam Lantinga  4/13/98

    Modified to write stdout/stderr to a message box at shutdown by Ripper  2007-12-27

    The WinMain function -- calls your program's main() function
*/

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

# define DIR_SEPERATOR TEXT("/")
# include <direct.h>

/* Include the SDL main definition header */
#include "SDL.h"
#include "SDL_main.h"

#ifdef main
#  undef main
#endif /* main */

static const int EXIT_ERROR_OUT_OF_MEMORY = 255;

/* Parse a command line buffer into arguments */
static int ParseCommandLine( TCHAR *cmdline,  TCHAR **argv)
{
	 TCHAR *bufp;
	int argc = 0;

	for ( bufp = cmdline; *bufp; ) 
	{
		/* Skip leading whitespace */
		while ( _istspace(*bufp) ) 
			++bufp;
		/* Skip over argument */
		if ( *bufp == _T('"') ) 
		{
			++bufp;
			if ( *bufp ) 
			{
				if ( argv ) 
					argv[argc] = bufp;
				++argc;
			}
			/* Skip over word */
			while ( *bufp && (*bufp != _T('"')) ) 
				++bufp;
		} 
		else 
		{
			if ( *bufp ) 
			{
				if ( argv ) 
					argv[argc] = bufp;
				++argc;
			}
			/* Skip over word */
			while ( *bufp && ! _istspace(*bufp) ) 
				++bufp;
		}
		if ( *bufp ) 
		{
			if ( argv ) 
				*bufp = _T('\0');
			++bufp;
		}
	}
	if ( argv ) 
		argv[argc] = NULL;
	return(argc);
}

/* Show an error message */
static void ShowError(const char *title, const char *message)
{
	fprintf(stderr, "%s: %s\n", title, message);
}

/* Pop up an out of memory message, returns to Windows */
static BOOL OutOfMemory(void)
{
	MessageBox(nullptr, _T("Fatal error: out of memory, aborting."), _T("Automatic Wolfenstein"), MB_OK | MB_ICONERROR);
	return EXIT_ERROR_OUT_OF_MEMORY;
}

/* SDL_Quit() shouldn't be used with atexit() directly because
   calling conventions may differ... */
static void cleanup(void)
{
	SDL_Quit();
}

/* Remove the output files if there was no output written */
static void cleanup_output(void)
{

	/* Flush the output in case anything is queued */
	fclose(stdout);
	fclose(stderr);
}

/* This is where execution begins [console apps] */
int console_main(int argc, TCHAR *argv[])
{
	size_t n;
	TCHAR *bufp, *appname;
	int status;

	/* Get the class name from argv[0] */
	appname = argv[0];

	if ( (bufp = _tcsrchr(argv[0], _T('\\'))) != NULL ) 
	{
		appname = bufp+1;
	} 
	else if ((bufp = _tcsrchr(argv[0], _T('/'))) != NULL)
	{
		appname = bufp+1;
	}

	if ((bufp = _tcsrchr(appname, _T('.'))) == NULL)
		n = _tcslen(appname);
	else
		n = (bufp-appname);

	bufp = SDL_stack_alloc(TCHAR, n+1);
	if ( bufp == NULL ) 
	{
		return OutOfMemory();
	}
	memset(bufp, 0, (n + 1) * sizeof(TCHAR));
	_tcsncpy(bufp, appname, n + 1);
	appname = bufp;

	/* Load SDL dynamic link library */
	if ( SDL_Init(SDL_INIT_NOPARACHUTE) < 0 ) 
	{
		// FIXME: ascii dependent
		MessageBoxA(nullptr, SDL_GetError(), "AutoWolf Winmain error", MB_OK | MB_ICONEXCLAMATION);
		return(FALSE);
	}
	atexit(cleanup_output);

	/* Sam:
	   We still need to pass in the application handle so that
	   DirectInput will initialize properly when SDL_RegisterApp()
	   is called later in the video initialization.
	 */
	SDL_SetModuleHandle(GetModuleHandle(NULL));

	/* Run the application main() code */
	status = SDL_main(argc, argv);

	/* Exit cleanly, calling atexit() functions */
	exit(status);

	/* Hush little compiler, don't you cry... */
	return 0;
}

/* This is where execution begins [windowed apps] */
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw)
{
	TCHAR **argv;
	int argc;
	TCHAR *cmdline;
	LPTSTR bufp;
	size_t nLen;

	/* Grab the command line */
	bufp = GetCommandLine();
	nLen = _tcslen(bufp) + 1;
	cmdline = SDL_stack_alloc(TCHAR, nLen);
	if ( cmdline == nullptr ) 
	{
		return OutOfMemory();
	}
	memset(cmdline, 0, nLen * sizeof(TCHAR));
	_tcsncpy(cmdline, bufp, nLen);

	/* Parse it into argv and argc */
	argc = ParseCommandLine(cmdline, NULL);
	argv = SDL_stack_alloc(TCHAR*, argc + 1);
	if ( argv == nullptr ) 
	{
		return OutOfMemory();
	}
	ParseCommandLine(cmdline, argv);

	/* Run the main program (after a little SDL initialization) */
	console_main(argc, argv);

	/* Hush little compiler, don't you cry... */
	return 0;
}

#endif  // _WIN32
