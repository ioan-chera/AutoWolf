/////////////////////////////////////////////////
//
// MUSE Header for .WL6
// Created Tue Jul 14 15:04:53 1992
//
/////////////////////////////////////////////////

//
// Sound names & indexes
//

// IOANCH 20130202: unification process
#ifndef AUDIOWL6_H_
#define AUDIOWL6_H_

typedef enum {
	HITWALLSND_wl6,              // 0
	SELECTWPNSND_wl6,            // 1
	SELECTITEMSND_wl6,           // 2
	HEARTBEATSND_wl6,            // 3
	MOVEGUN2SND_wl6,             // 4
	MOVEGUN1SND_wl6,             // 5
	NOWAYSND_wl6,                // 6
	NAZIHITPLAYERSND_wl6,        // 7
	SCHABBSTHROWSND_wl6,         // 8
	PLAYERDEATHSND_wl6,          // 9
	DOGDEATHSND_wl6,             // 10
	ATKGATLINGSND_wl6,           // 11
	GETKEYSND_wl6,               // 12
	NOITEMSND_wl6,               // 13
	WALK1SND_wl6,                // 14
	WALK2SND_wl6,                // 15
	TAKEDAMAGESND_wl6,           // 16
	GAMEOVERSND_wl6,             // 17
	OPENDOORSND_wl6,             // 18
	CLOSEDOORSND_wl6,            // 19
	DONOTHINGSND_wl6,            // 20
	HALTSND_wl6,                 // 21
	DEATHSCREAM2SND_wl6,         // 22
	ATKKNIFESND_wl6,             // 23
	ATKPISTOLSND_wl6,            // 24
	DEATHSCREAM3SND_wl6,         // 25
	ATKMACHINEGUNSND_wl6,        // 26
	HITENEMYSND_wl6,             // 27
	SHOOTDOORSND_wl6,            // 28
	DEATHSCREAM1SND_wl6,         // 29
	GETMACHINESND_wl6,           // 30
	GETAMMOSND_wl6,              // 31
	SHOOTSND_wl6,                // 32
	HEALTH1SND_wl6,              // 33
	HEALTH2SND_wl6,              // 34
	BONUS1SND_wl6,               // 35
	BONUS2SND_wl6,               // 36
	BONUS3SND_wl6,               // 37
	GETGATLINGSND_wl6,           // 38
	ESCPRESSEDSND_wl6,           // 39
	LEVELDONESND_wl6,            // 40
	DOGBARKSND_wl6,              // 41
	ENDBONUS1SND_wl6,            // 42
	ENDBONUS2SND_wl6,            // 43
	BONUS1UPSND_wl6,             // 44
	BONUS4SND_wl6,               // 45
	PUSHWALLSND_wl6,             // 46
	NOBONUSSND_wl6,              // 47
	PERCENT100SND_wl6,           // 48
	BOSSACTIVESND_wl6,           // 49
	MUTTISND_wl6,                // 50
	SCHUTZADSND_wl6,             // 51
	AHHHGSND_wl6,                // 52
	DIESND_wl6,                  // 53
	EVASND_wl6,                  // 54
	GUTENTAGSND_wl6,             // 55
	LEBENSND_wl6,                // 56
	SCHEISTSND_wl6,              // 57
	NAZIFIRESND_wl6,             // 58
	BOSSFIRESND_wl6,             // 59
	SSFIRESND_wl6,               // 60
	SLURPIESND_wl6,              // 61
	TOT_HUNDSND_wl6,             // 62
	MEINGOTTSND_wl6,             // 63
	SCHABBSHASND_wl6,            // 64
	HITLERHASND_wl6,             // 65
	SPIONSND_wl6,                // 66
	NEINSOVASSND_wl6,            // 67
	DOGATTACKSND_wl6,            // 68
	FLAMETHROWERSND_wl6,         // 69
	MECHSTEPSND_wl6,             // 70
	GOOBSSND_wl6,                // 71
	YEAHSND_wl6,                 // 72
    // IOANCH 20130301: unification culling

	DEATHSCREAM4SND_wl6,         // 73
	DEATHSCREAM5SND_wl6,         // 74
	DEATHSCREAM6SND_wl6,         // 75
	DEATHSCREAM7SND_wl6,         // 76
	DEATHSCREAM8SND_wl6,         // 77
	DEATHSCREAM9SND_wl6,         // 78
	DONNERSND_wl6,               // 79
	EINESND_wl6,                 // 80
	ERLAUBENSND_wl6,             // 81
	KEINSND_wl6,                 // 82
	MEINSND_wl6,                 // 83
	ROSESND_wl6,                 // 84
	MISSILEFIRESND_wl6,          // 85
	MISSILEHITSND_wl6,           // 86

	LASTSOUND_wl6
} soundnames_wl6;

//
// Base offsets
//
#define STARTPCSOUNDS_wl6		0
#define STARTADLIBSOUNDS_wl6	LASTSOUND_wl6
#define STARTDIGISOUNDS_wl6		(2*LASTSOUND_wl6)
#define STARTMUSIC_wl6		    (3*LASTSOUND_wl6)

//
// Music names & indexes
//
typedef enum {
	CORNER_MUS_wl6,              // 0
	DUNGEON_MUS_wl6,             // 1
	WARMARCH_MUS_wl6,            // 2
	GETTHEM_MUS_wl6,             // 3
	HEADACHE_MUS_wl6,            // 4
	HITLWLTZ_MUS_wl6,            // 5
	INTROCW3_MUS_wl6,            // 6
	NAZI_NOR_MUS_wl6,            // 7
	NAZI_OMI_MUS_wl6,            // 8
	POW_MUS_wl6,                 // 9
	SALUTE_MUS_wl6,              // 10
	SEARCHN_MUS_wl6,             // 11
	SUSPENSE_MUS_wl6,            // 12
	VICTORS_MUS_wl6,             // 13
	WONDERIN_MUS_wl6,            // 14
	FUNKYOU_MUS_wl6,             // 15
	ENDLEVEL_MUS_wl6,            // 16
	GOINGAFT_MUS_wl6,            // 17
	PREGNANT_MUS_wl6,            // 18
	ULTIMATE_MUS_wl6,            // 19
	NAZI_RAP_MUS_wl6,            // 20
	ZEROHOUR_MUS_wl6,            // 21
	TWELFTH_MUS_wl6,             // 22
	ROSTER_MUS_wl6,              // 23
	URAHERO_MUS_wl6,             // 24
	VICMARCH_MUS_wl6,            // 25
	PACMAN_MUS_wl6,              // 26
	LASTMUSIC_wl6
} musicnames_wl6;

#define NUMSOUNDS_wl6		    LASTSOUND_wl6
#define NUMSNDCHUNKS_wl6		(STARTMUSIC_wl6 + LASTMUSIC_wl6)

/////////////////////////////////////////////////
//
// Thanks for playing with MUSE!
//
/////////////////////////////////////////////////
#endif
