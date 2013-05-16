//
// Copyright (C) 2013  Ioan Chera
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


//
//  audioabstract.h
//  Wolf4SDL
//
//  Created by ioan on 03.02.2013.
//
//

#ifndef Wolf4SDL_audioabstract_h
#define Wolf4SDL_audioabstract_h

#include "audiowl6.h"
#include "audiosod.h"


//
// soundnames
//
// List of generic (abstract) names
//
enum soundnames {
	AHHHGSND,                // 52 DIGI
	ANGELDEATHSND,           // 77 DIGI
	ANGELFIRESND,            // 69
	ANGELSIGHTSND,           // 65 DIGI
	ANGELTIREDSND,           // 80
	ATKGATLINGSND,           // 11 DIGI
	ATKKNIFESND,             // 23
	ATKMACHINEGUNSND,        // 26 DIGI
	ATKPISTOLSND,            // 24 DIGI
	BONUS1SND,               // 35
	BONUS1UPSND,             // 44
	BONUS2SND,               // 36
	BONUS3SND,               // 37
	BONUS4SND,               // 45
	BOSSACTIVESND,           // 49
	BOSSFIRESND,             // 59 DIGI
	CLOSEDOORSND,            // 19 DIGI
	DEATHSCREAM1SND,         // 29 DIGI
	DEATHSCREAM2SND,         // 22 DIGI
	DEATHSCREAM3SND,         // 25 DIGI
	DEATHSCREAM4SND,         // 73
	DEATHSCREAM5SND,         // 74
	DEATHSCREAM6SND,         // 75
	DEATHSCREAM7SND,         // 76
	DEATHSCREAM8SND,         // 77
	DEATHSCREAM9SND,         // 78
	DIESND,                  // 53
	DOGATTACKSND,            // 68 DIGI
	DOGBARKSND,              // 41 DIGI
	DOGDEATHSND,             // 10 DIGI
	DONNERSND,               // 79
	DONOTHINGSND,            // 20
	EINESND,                 // 80
	ENDBONUS1SND,            // 42
	ENDBONUS2SND,            // 43
	ERLAUBENSND,             // 81
	ESCPRESSEDSND,           // 39
	EVASND,                  // 54
	FLAMETHROWERSND,         // 69
	GAMEOVERSND,             // 17
	GETAMMOBOXSND,           // 64
	GETAMMOSND,              // 31
	GETGATLINGSND,           // 38 DIGI SPEAR()
	GETKEYSND,               // 12
	GETMACHINESND,           // 30
	GETSPEARSND,             // 79 DIGI
	GHOSTFADESND,            // 62
    GHOSTSIGHTSND,           // 3
	GOOBSSND,                // 71
	GUTENTAGSND,             // 55
	HALTSND,                 // 21 DIGI
	HEALTH1SND,              // 33
	HEALTH2SND,              // 34
	HEARTBEATSND,            // 3
	HITENEMYSND,             // 27
	HITLERHASND,             // 65
	HITWALLSND,              // 0
	KEINSND,                 // 82
	KNIGHTDEATHSND,          // 76 DIGI
	KNIGHTMISSILESND,        // 78
	KNIGHTSIGHTSND,          // 75 DIGI
	LEBENSND,                // 56 DIGI
	LEVELDONESND,            // 40 DIGI
	MECHSTEPSND,             // 70
	MEINGOTTSND,             // 63
	MEINSND,                 // 83
	MISSILEFIRESND,          // 85
	MISSILEHITSND,           // 86
	MOVEGUN1SND,             // 5
	MOVEGUN2SND,             // 4
	MUTTISND,                // 50
	NAZIFIRESND,             // 58 DIGI
	NAZIHITPLAYERSND,        // 7
	NEINSOVASSND,            // 67 DIGI
	NOBONUSSND,              // 47
	NOITEMSND,               // 13
	NOWAYSND,                // 6
	OPENDOORSND,             // 18 DIGI
	PERCENT100SND,           // 48
	PLAYERDEATHSND,          // 9
	PUSHWALLSND,             // 46 DIGI
	ROSESND,                 // 84
	SCHABBSHASND,            // 64
	SCHABBSTHROWSND,         // 8
	SCHEISTSND,              // 57
	SCHUTZADSND,             // 51 DIGI
	SELECTITEMSND,           // 2
	SELECTWPNSND,            // 1
	SHOOTDOORSND,            // 28
	SHOOTSND,                // 32
	SLURPIESND,              // 61 DIGI
	SPIONSND,                // 66 DIGI
	SSFIRESND,               // 60 DIGI
	TAKEDAMAGESND,           // 16 DIGI
	TOT_HUNDSND,             // 62
	TRANSDEATHSND,           // 71 DIGI
	TRANSSIGHTSND,           // 70 DIGI
	UBERDEATHSND,            // 74 DIGI
	WALK1SND,                // 14
	WALK2SND,                // 15
	WILHELMDEATHSND,         // 73 DIGI
	WILHELMSIGHTSND,         // 72 DIGI
	YEAHSND,                 // 72
    LASTSNDABSTRACT
} ;

#endif
