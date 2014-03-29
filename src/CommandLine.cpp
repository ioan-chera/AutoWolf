// Copyright (C) 2014  Ioan Chera
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
////////////////////////////////////////////////////////////////////////////////
//
// Command line manager
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#include <Windows.h>
#include <shellapi.h>
#endif

#include <system_error>
#include <vector>

#include "CommandLine.h"
#include "Config.h"
#include "Exception.h"
#include "i_system.h"
#include "ShellUnicode.h"
#include "StdStringExtensions.h"
#include "wl_def.h"
#include "PString.h"

std::vector<std::string> s_argv;

#ifdef _WIN32
static void feedFromWindows()
{
#error TODO Windows: memory management of argvW
	int argc;
	LPWSTR* argvW = CommandLineToArgvW(GetCommandLineW(), &argc);
	if (!argvW)
		throw std::system_error(std::error_code(GetLastError(), std::system_category()), "Command line parse error");

	// Convert this crap to UTF8 and back to wchar later
	s_argv.reserve(argc);
	for (int i = 0; i < argc; ++i)
	{
		s_argv.push_back(WideCharToUTF8(argvW[i]));
	}
}
#endif

void CommandLine::Feed(int argc, const char* const* argv)
{
#ifdef _WIN32
	feedFromWindows();
#else
	if (!argc || !argv)
		return;
	for (int i = 0; i < argc; ++i)
		s_argv.push_back(argv[i]);
#endif
}

static void checkEnvVars()
{
	std::string wolfdir = ShellUnicode::getenv("AUTOWOLFDIR");
	if (!wolfdir.empty())
	{
		cfg_wolfdir = wolfdir;
	}
}

void CommandLine::Parse()
{
	bool sampleRateGiven = false, audioBufferGiven = false;
	int defaultSampleRate = cfg_samplerate;

	cfg_botActive = true;

	try
	{
		checkEnvVars();
		if (s_argv.begin() == s_argv.end())
			throw std::length_error("Strange situation: no arguments provided (not even the executable name)");
		for (auto it = s_argv.begin() + 1; it != s_argv.end(); ++it)
		{
			std::string &arg = *it;
			// IOANCH 20130303: unification
			if (!strcasecmp(arg, "--debugmode") || !strcasecmp(arg, "--goobers"))
				cfg_debugmode = true;
			else if(!strcasecmp(arg, "--baby"))
				cfg_difficulty = 0;
			else if(!strcasecmp(arg, "--easy"))
				cfg_difficulty = 1;
			else if (!strcasecmp(arg, "--normal"))
				cfg_difficulty = 2;
			else if (!strcasecmp(arg, "--hard"))
				cfg_difficulty = 3;
			else if (!strcasecmp(arg, "--nowait"))
				cfg_nowait = true;
			else if (!strcasecmp(arg, "--tedlevel"))
			{
				if (++it == s_argv.end())
					throw std::range_error("The tedlevel option is missing the level argument!");
				else
					cfg_tedlevel = atoi(it->c_str());
			}
			else if (!strcasecmp(arg, "--windowed"))
				cfg_fullscreen = false;
			else if (!strcasecmp(arg, "--fullscreen"))
				cfg_fullscreen = true;
				// IOANCH 20121611: added --fullscreen option
			else if (!strcasecmp(arg, "--windowed-mouse"))
			{
				cfg_fullscreen = false;
				cfg_forcegrabmouse = true;
			}
			else if (!strcasecmp(arg, "--res"))
			{
				if (it + 2 >= s_argv.end())
					throw std::range_error("The res option needs the width and/or the height argument!");
				else
				{
					cfg_screenWidth = atoi((++it)->c_str());
					cfg_screenHeight = atoi((++it)->c_str());
					unsigned factor = cfg_screenWidth / 320;
					if (cfg_screenWidth % 320 ||
						(cfg_screenHeight != 200 * factor &&
						cfg_screenHeight != 240 * factor))
					{
						throw std::range_error("Screen size must be a multiple of 320x200 or 320x240!");
					}
				}
			}
			else if (!strcasecmp(arg, "--resf"))
			{
				if (it + 2 >= s_argv.end())
					throw std::range_error("The resf option needs the width and/or the height argument!");
				else
				{
					cfg_screenWidth = atoi((++it)->c_str());
					cfg_screenHeight = atoi((++it)->c_str());
					if (cfg_screenWidth < 320)
						throw std::range_error("Screen width must be at least 320!");
					if (cfg_screenHeight < 200)
						throw std::range_error("Screen height must be at least 200!");
				}
			}
			else if (!strcasecmp(arg, "--bits"))
			{
				if (++it >= s_argv.end())
					throw std::range_error("The bits option is missing the color depth argument!");
				else
				{
					cfg_screenBits = atoi(it->c_str());
					switch (cfg_screenBits)
					{
					case 8:
					case 16:
					case 24:
					case 32:
						break;

					default:
						throw std::range_error("Screen color depth must be 8, 16, 24, or 32!");
						break;
					}
				}
			}
			else if(!strcasecmp(arg, "--nodblbuf"))
				cfg_usedoublebuffering = false;
			else if (!strcasecmp(arg, "--extravbls"))
			{
				if (++it >= s_argv.end())
					throw std::range_error("The extravbls option is missing the vbls argument!");
				else
				{
					cfg_extravbls = atoi(it->c_str());
					if (cfg_extravbls < 0)
						throw std::range_error("Extravbls must be positive!");
				}
			}
			else if (!strcasecmp(arg, "--joystick"))
			{
				if (++it >= s_argv.end())
					throw std::range_error("The joystick option is missing the index argument!");
				else
					cfg_joystickindex = atoi(it->c_str());
				// index is checked in InitGame
			}
			else if (!strcasecmp(arg, "--joystickhat"))
			{
				if (++it >= s_argv.end())
					throw std::range_error("The joystickhat option is missing the "
					"index argument!");
				else
					cfg_joystickhat = atoi(it->c_str());
			}
			else if (!strcasecmp(arg, "--samplerate"))
			{
				if (++it >= s_argv.end())
					throw std::range_error("The samplerate option is missing the "
					"rate argument!");
				else
					cfg_samplerate = atoi(it->c_str());
				sampleRateGiven = true;
			}
			else if (!strcasecmp(arg, "--audiobuffer"))
			{
				if (++it >= s_argv.end())
					throw std::range_error("The audiobuffer option is missing the "
					"size argument!");
				else
					cfg_audiobuffer = atoi(it->c_str());
				audioBufferGiven = true;
			}
			else if (!strcasecmp(arg, "--mission"))
			{
				if (++it >= s_argv.end())
					throw std::range_error("The mission option is missing the "
					"mission argument!");
				else
				{
					cfg_mission = atoi(it->c_str());
					if (cfg_mission < 0 || cfg_mission > 3)
						throw std::range_error("The mission option must be between 0 "
						"and 3!");
				}
			}
			else if (!strcasecmp(arg, "--configdir"))
			{
				if (++it >= s_argv.end())
					throw std::range_error("The configdir option is missing the dir "
					"argument!");
				else
				{
					cfg_dir = arg;
				}
			}
			else if (!strcasecmp(arg, "--ignorenumchunks"))
				cfg_ignorenumchunks = true;
			else if (!strcasecmp(arg, "--help"))
				throw Exception("Showing help");
				// IOANCH 17.05.2012: added --nobot parameter
			else if (!strcasecmp(arg, "--nobot"))
				cfg_botActive = false;
				// IOANCH 17.05.2012: added --nonazis
			else if (!strcasecmp(arg, "--nonazis"))
				cfg_nonazis = true;
				// IOANCH 29.09.2012: added --secretstep3
			else if (!strcasecmp(arg, "--secretstep3"))
				cfg_secretstep3 = true;
			else if (!strcasecmp(arg, "--wolfdir"))
			{
				// IOANCH 20130304: added --wolfdir
				if (++it >= s_argv.end())
					throw std::range_error("The wolfdir option is missing the dir argument!");
				else
				{
					cfg_wolfdir = *it;
				}
			}
			else
				throw Exception((std::string("Unknown argument: ") + arg + "\n").c_str());
		}
	}
	catch (const std::exception &e)
	{
		std::string help;
		help += e.what();
		help += "\n";
		help +=
			"AutoWolf v1\n"
			"By Ioan Chera on Wolf4SDL codebase\n"
			"Wolf4SDL: Ported by Chaos-Software (http://www.chaos-software.de.vu)\n"
			"Original Wolfenstein 3D by id Software\n\n";
#ifdef __APPLE__
		help += "Usage: open -a AutoWolf --args [options]\n";
#else
		help += "Usage: autowolf [options]\n";
#endif
		help += "Options:\n"
			" --nobot                Do not use bot\n"
			" --nonazis              Maps without Nazis spawned\n"
			" --secretstep3          Emulate 3-step secret wall bug\n"
			" --help                 This help page\n"
			" --tedlevel <level>     Starts the game in the given level\n"
			" --baby                 Sets the difficulty to baby for tedlevel\n"
			" --easy                 Sets the difficulty to easy for tedlevel\n"
			" --normal               Sets the difficulty to normal for tedlevel\n"
			" --hard                 Sets the difficulty to hard for tedlevel\n"
			" --nowait               Skips intro screens\n"
			" --windowed[-mouse]     Starts the game in a window [and grabs mouse]\n"
			" --res <width> <height> Sets the screen resolution\n"
			"                        (must be multiple of 320x200 or 320x240)\n"
			" --resf <w> <h>         Sets any screen resolution >= 320x200\n"
			"                        (which may result in graphic errors)\n"
			" --bits <b>             Sets the screen color depth\n"
			"                        (use this when you have palette/fading problems\n"
			"                        allowed: 8, 16, 24, 32, default: \"best\" depth)\n"
			" --nodblbuf             Don't use SDL's double buffering\n"
			" --extravbls <vbls>     Sets a delay after each frame, which may help to\n"
			"                        reduce flickering (unit is currently 8 ms, default: 0)\n"
			" --joystick <index>     Use the index-th joystick if available\n"
			"                        (-1 to disable joystick, default: 0)\n"
			" --joystickhat <index>  Enables movement with the given coolie hat\n"
			" --samplerate <rate>    Sets the sound sample rate (given in Hz, default: ";
		help += defaultSampleRate;
		help += ")\n"
			" --audiobuffer <size>   Sets the size of the audio buffer (-> sound latency)\n"
			"                        (given in bytes, default: 2048 / (44100 / samplerate))\n"
			" --ignorenumchunks      Ignores the number of chunks in VGAHEAD.*\n"
			"                        (may be useful for some broken mods)\n"
			" --configdir <dir>      Directory where config file and save games are stored\n";
#if defined(_arch_dreamcast) || defined(_WIN32)
		help += "                        (default: current directory)\n";
#elif !defined(__APPLE__)
		help += "                        (default: $HOME/.autowolf)\n";	// IOANCH 20130116: changed name
#else
		help += "                        (default: ~/Library/Application Support/com.ichera.autowolf)\n";
#endif
			// IOANCH 20130301: unification culling
		help +=
			" --mission <mission>    Mission number to play (0-3)\n"
			"                        (default: 0 -> .SOD, 1-3 -> .SD*)\n";
		throw Exception(help.c_str());
	}

	if (sampleRateGiven && !audioBufferGiven)
		cfg_audiobuffer = 2048 / (44100 / cfg_samplerate);
}