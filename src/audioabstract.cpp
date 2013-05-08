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
//  audioabstract.c
//  Wolf4SDL
//
//  Created by ioan on 03.02.2013.
//
//

#include "audioabstract.h"

//
// soundmap
//
// Maps generic (abstract) sound names to WL6 or SOD (the only different modes)
//
const unsigned int soundmap[][2] =
{
    {AHHHGSND_wl6,          AHHHGSND_sod},
	{0,                     ANGELDEATHSND_sod},
	{0,                     ANGELFIRESND_sod},
	{0,                     ANGELSIGHTSND_sod},
	{0,                     ANGELTIREDSND_sod},
	{ATKGATLINGSND_wl6,     ATKGATLINGSND_sod},
	{ATKKNIFESND_wl6,       ATKKNIFESND_sod},
	{ATKMACHINEGUNSND_wl6,  ATKMACHINEGUNSND_sod},
	{ATKPISTOLSND_wl6,      ATKPISTOLSND_sod},
	{BONUS1SND_wl6,         BONUS1SND_sod},
	{BONUS1UPSND_wl6,       BONUS1UPSND_sod},
	{BONUS2SND_wl6,         BONUS2SND_sod},
	{BONUS3SND_wl6,         BONUS3SND_sod},
	{BONUS4SND_wl6,         BONUS4SND_sod},
	{BOSSACTIVESND_wl6,     BOSSACTIVESND_sod},
	{BOSSFIRESND_wl6,       BOSSFIRESND_sod},
	{CLOSEDOORSND_wl6,      CLOSEDOORSND_sod},
	{DEATHSCREAM1SND_wl6,   DEATHSCREAM1SND_sod},
	{DEATHSCREAM2SND_wl6,   DEATHSCREAM2SND_sod},
	{DEATHSCREAM3SND_wl6,   DEATHSCREAM3SND_sod},
	{DEATHSCREAM4SND_wl6,   DEATHSCREAM4SND_sod},
	{DEATHSCREAM5SND_wl6,   DEATHSCREAM5SND_sod},
	{DEATHSCREAM6SND_wl6,   DEATHSCREAM6SND_sod},
	{DEATHSCREAM7SND_wl6,   DEATHSCREAM7SND_sod},
	{DEATHSCREAM8SND_wl6,   DEATHSCREAM8SND_sod},
	{DEATHSCREAM9SND_wl6,   DEATHSCREAM9SND_sod},
	{DIESND_wl6,            0},
	{DOGATTACKSND_wl6,      DOGATTACKSND_sod},
	{DOGBARKSND_wl6,        DOGBARKSND_sod},
	{DOGDEATHSND_wl6,       DOGDEATHSND_sod},
	{DONNERSND_wl6,         0},
	{DONOTHINGSND_wl6,      DONOTHINGSND_sod},
	{EINESND_wl6,           0},
	{ENDBONUS1SND_wl6,      ENDBONUS1SND_sod},
	{ENDBONUS2SND_wl6,      ENDBONUS2SND_sod},
	{ERLAUBENSND_wl6,       0},
	{ESCPRESSEDSND_wl6,     ESCPRESSEDSND_sod},
	{EVASND_wl6,            0},
	{FLAMETHROWERSND_wl6,   0},
	{GAMEOVERSND_wl6,       GAMEOVERSND_sod},
	{0,                     GETAMMOBOXSND_sod},
	{GETAMMOSND_wl6,        GETAMMOSND_sod},
	{GETGATLINGSND_wl6,     GETGATLINGSND_sod},
	{GETKEYSND_wl6,         GETKEYSND_sod},
	{GETMACHINESND_wl6,     GETMACHINESND_sod},
	{0,                     GETSPEARSND_sod},
	{0,                     GHOSTFADESND_sod},
    {0,                     GHOSTSIGHTSND_sod},
	{GOOBSSND_wl6,          0},
	{GUTENTAGSND_wl6,       0}, 
	{HALTSND_wl6,           HALTSND_sod},
	{HEALTH1SND_wl6,        HEALTH1SND_sod},
	{HEALTH2SND_wl6,        HEALTH2SND_sod},
	{HEARTBEATSND_wl6,      0},
	{HITENEMYSND_wl6,       HITENEMYSND_sod},
	{HITLERHASND_wl6,       0},
	{HITWALLSND_wl6,        HITWALLSND_sod},
	{KEINSND_wl6,           0},
	{0,                     KNIGHTDEATHSND_sod},
	{0,                     KNIGHTMISSILESND_sod},
	{0,                     KNIGHTSIGHTSND_sod},
	{LEBENSND_wl6,          LEBENSND_sod},
	{LEVELDONESND_wl6,      LEVELDONESND_sod},
	{MECHSTEPSND_wl6,       0},
	{MEINGOTTSND_wl6,       0},
	{MEINSND_wl6,           0},
	{MISSILEFIRESND_wl6,    MISSILEFIRESND_sod},
	{MISSILEHITSND_wl6,     MISSILEHITSND_sod},
	{MOVEGUN1SND_wl6,       MOVEGUN1SND_sod},
	{MOVEGUN2SND_wl6,       MOVEGUN2SND_sod},
	{MUTTISND_wl6,          0},
	{NAZIFIRESND_wl6,       NAZIFIRESND_sod},
	{NAZIHITPLAYERSND_wl6,  NAZIHITPLAYERSND_sod},
	{NEINSOVASSND_wl6,      NEINSOVASSND_sod},
	{NOBONUSSND_wl6,        NOBONUSSND_sod},
	{NOITEMSND_wl6,         NOITEMSND_sod},
	{NOWAYSND_wl6,          NOWAYSND_sod},
	{OPENDOORSND_wl6,       OPENDOORSND_sod},
	{PERCENT100SND_wl6,     PERCENT100SND_sod},
	{PLAYERDEATHSND_wl6,    PLAYERDEATHSND_sod},
	{PUSHWALLSND_wl6,       PUSHWALLSND_sod},
	{ROSESND_wl6,           0},
	{SCHABBSHASND_wl6,      0},
	{SCHABBSTHROWSND_wl6,   0},
	{SCHEISTSND_wl6,        0},
	{SCHUTZADSND_wl6,       SCHUTZADSND_sod},
	{SELECTITEMSND_wl6,     SELECTITEMSND_sod},
	{SELECTWPNSND_wl6,      0},
	{SHOOTDOORSND_wl6,      SHOOTDOORSND_sod},
	{SHOOTSND_wl6,          SHOOTSND_sod},
	{SLURPIESND_wl6,        SLURPIESND_sod},
	{SPIONSND_wl6,          SPIONSND_sod},
	{SSFIRESND_wl6,         SSFIRESND_sod},
	{TAKEDAMAGESND_wl6,     TAKEDAMAGESND_sod},
	{TOT_HUNDSND_wl6,       0},
	{0,                     TRANSDEATHSND_sod},
	{0,                     TRANSSIGHTSND_sod},
	{0,                     UBERDEATHSND_sod},
	{WALK1SND_wl6,          WALK1SND_sod},
	{WALK2SND_wl6,          WALK2SND_sod},
	{0,                     WILHELMDEATHSND_sod},
	{0,                     WILHELMSIGHTSND_sod},
	{YEAHSND_wl6,           0},
};