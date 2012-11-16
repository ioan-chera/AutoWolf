Automatic Wolfenstein alias AutoWolf by Ioan Chera (https://sourceforge.net/users/printz)
Wolf4SDL by Moritz "Ripper" Kroll (http://www.chaos-software.de.vu)
Original Wolfenstein 3D by id Software (http://www.idsoftware.com)
=============================================================================

AutoWolf is a modification of Ripper's Wolf4SDL port of id Software's classic
first-person shooter Wolfenstein 3D, with the purpose of implementing
competitive bots (computer controlled players) who should be able to solve any
level, at maximum score.

This version is alpha02. As such, there are some features that haven't been
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


Installing
----------
Windows or Mac OSX:

Extract the contents of the archive into a folder,
and place all Wolfenstein 3D data files (*.WL6 or *.SOD) in that same folder.

AutoWolf shall be ready to start.


Updates over alpha01
--------------------
 
 - The bot can now search for cover, not just by moving backwards. For this, 
   it strafes.
 - Smooth movement, no longer auto-levelled on the centre of the tile.
 - More conserving usage of ammo, by switching to the machinegun when wasteful
   with the chaingun.
 - Now it will not get stuck, waiting for user input, if a threat remains stuck
   nearby but out of sight. It will stop waiting and will move ahead.
 - Mac OSX binary available (Snow Leopard minimum required).


Future plans:
-------------

These features haven't been started or haven't been completed, and are probable
to appear on the next alpha or beta:

 - Screensaver that also lets the computer go into power saving mode.
 - Intelligent pushwall puzzle solving;
 - Integration into Blzut3's ECWolf port and therefore mod-aware behavior, as
   well as posibility to make the bot cooperative or deathmatch (if applicable);
 - Speed-running behaviour;
 - Optional human-like behaviour, especially if it happens in a proposed multiplayer;


New command-line parameters:
----------------------------

In addition to Wolf4SDL, AutoWolf supports the following command-line parameters:
 --nobot                Disables the bot. This makes the game act like regular
                        Wolfenstein.
 --nonazis              Disables spawning of enemies.
 --secretstep3          Emulates the Wolf3D bug of secret walls being pushed too far.
                        Necessary for any custom map which relies on this behaviour.

Compiling from source code:
---------------------------

See Wolf4SDL's readme file (README-wolf4sdl.txt) for information on this topic.

Frequently asked questions
--------------------------

Q: The bot has stopped moving, is this a problem with its AI?
A: The bot will stop moving when there is nowhere to go, i.e. when it's stuck.
   This can happen because:
   a) it failed to solve a pushwall puzzle critical to solving the level.
      Currently the bot does not know how to intelligently solve pushwall
      puzzles like those in E4M2 or E4M7 (the latter case being critical).
   b) the map has been designed with the pushwall bug in mind, where the
      walls would move 3 steps instead of 2, in vanilla Wolf3D, when the
      frame rate would be large enough. To emulate this bug, you need to
      run AutoWolf with the --secretstep3 command-line switch.


Q: The bot has gone into a perpetual loop. It won't advance.
A: Though I've tried fixing the more prominent cases, it can still occur.
   To budge it, simply try to move it forward (by running). Even if it's a
   bot, you still have some limited control over it.


Q: Does the bot learn as it goes?
A: No, for now everything is programmed from the start. This also means that
   nothing needs to be stored in save-games, since the bot can set itself up
   immediately from the level's conditions.
   Learning algorithms are intended when AutoWolf gets ported to mod-friendly
   engines.


Q: Hey, the bot seems to die quite often now!
A: Sometimes the bot may happen to get into quite risky behaviour, waking up
   too many Nazis and running back and forth into danger (surrounded) until
   it's capped to death. Even though I understand the error of this tactic,
   it makes the game more fun to watch.

Credits:
--------

 - id Software for Wolfenstein 3D, the pioneer of first-person shooters
   and one of the first computer games of my life.
 - Moritz "Ripper" Kroll for his excellent port of DOS Wolfenstein to modern
   systems, which made this bot possible, or at least with better chances of
   success.
 - SourceForge for their hosting of open-source projects.

License:
---------

This program is distributed under the GNU GENERAL PUBLIC LICENSE version 2.
See license-gpl.txt for details.

=============================================================================

    This program is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
                             for more details.

=============================================================================

Contact:
--------
You can email me for questions regarding this project at ioan_chera@yahoo.com
You can find me posting at the Doomworld or ZDoom forums as the user "printz".
PM me there if you're registered.