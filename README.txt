Automatic Wolfenstein alias AutoWolf by Ioan Chera "printz" (https://sourceforge.net/users/printz)
Wolf4SDL by Moritz "Ripper" Kroll (http://www.chaos-software.de.vu)
Original Wolfenstein 3D by id Software (http://www.idsoftware.com)
=============================================================================

AutoWolf is a modification of Ripper's Wolf4SDL port of id Software's classic
first-person shooter Wolfenstein 3D, with the purpose of implementing
competitive bots (computer controlled players) who should be able to solve any
level, at maximum score.

This version is alpha01. As such, there are some features that haven't been
completed, but the bot should be able to survive and finish most of the maps,
and even win some of the boss setups.

This port, just like Wolf4SDL, comes in several variants, for each suitable
Wolf3D version: 1.1, 1.4 Apogee, 1.4 Activision, Spear of Destiny. Make sure
you use the right one for the Wolfenstein version you have. Otherwise, you'll
see errors at startup and the program will abort.

You can use this bot with mods too, as long as said mods do not have their own
Wolf3D executables. If they have their own executables, it means you'll have to
run those instead of AutoWolf for the game to work properly, and I haven't
developed any way for the bot to work independent of the Wolf3D executable.

Project homepage:
https://sourceforge.net/projects/autowolf/


Future plans:
-------------

These features haven't been started or haven't been completed, and are probable
to appear on the next alpha or beta:

 - Improved cover, knife and projectile evasion tactics;
 - Intelligent pushwall puzzle solving;
 - Integration into Blzut3's ECWolf port and therefore mod-aware behavior, as
   well as posibility to make the bot cooperative or deathmatch (if applicable);
 - Speed-running behaviour;
 - Optional human-like behaviour, especially if it happens in a proposed multiplayer;
 - Make a cross-platform binary release, including on Mac OSX (I might be having
   an outdated OSX for now).


New command-line parameters:
----------------------------

In addition to Wolf4SDL, AutoWolf supports the following command-line parameters:
 --nobot                Disables the bot. This makes the game act like regular
                        Wolfenstein.
 --nonazis              Disables spawning of enemies.

Compiling from source code:
---------------------------

See Wolf4SDL's readme file (README-wolf4sdl.txt) for information on this topic.


Credits:
--------

 - id Software for Wolfenstein 3D, the pioneer of first-person shooters
   and one of the first computer games of my life.
 - Moritz "Ripper" Kroll for his excellent port of DOS Wolfenstein to modern
   systems, which made this bot possible, or at least with better chances of
   success.
 - Microsoft for Visual Studio 2008 Professional and their Academic Alliance
   with the universities.
 - All the people involved in computer science, from school, from Wikipedia,
   for teaching me some useful graph traversing algorithms.

Licenses:
---------

 - The original source code of Wolfenstein 3D:
     At your choice:
     - license-id.txt or
     - license-gpl.txt
 - The OPL2 emulator:
     At your choice:
     - license-mame.txt (fmopl.cpp)
     - license-gpl.txt (dbopl.cpp, USE_GPL define in version.h or set GPL=1 for Makefile)

Contact:
--------
You can email me for questions regarding this project at ioan_chera@yahoo.com
You can find me posting at the Doomworld or ZDoom forums as the user "printz"