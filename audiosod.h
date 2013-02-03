/////////////////////////////////////////////////
//
// MUSE Header for .SOD
// Created Thu Aug 13 09:25:58 1992
//
/////////////////////////////////////////////////

// IOANCH 20130202: reordered some macros

//
// Sound names & indexes
//

// IOANCH 20130202: unification process
typedef enum {
	HITWALLSND_sod,              // 0
	MISSILEHITSND_sod,           // 1
	SELECTITEMSND_sod,           // 2
	GHOSTSIGHTSND_sod,           // 3
	MOVEGUN2SND_sod,             // 4
	MOVEGUN1SND_sod,             // 5
	NOWAYSND_sod,                // 6
	NAZIHITPLAYERSND_sod,        // 7
	MISSILEFIRESND_sod,          // 8
	PLAYERDEATHSND_sod,          // 9
	DOGDEATHSND_sod,             // 10
	ATKGATLINGSND_sod,           // 11
	GETKEYSND_sod,               // 12
	NOITEMSND_sod,               // 13
	WALK1SND_sod,                // 14
	WALK2SND_sod,                // 15
	TAKEDAMAGESND_sod,           // 16
	GAMEOVERSND_sod,             // 17
	OPENDOORSND_sod,             // 18
	CLOSEDOORSND_sod,            // 19
	DONOTHINGSND_sod,            // 20
	HALTSND_sod,                 // 21
	DEATHSCREAM2SND_sod,         // 22
	ATKKNIFESND_sod,             // 23
	ATKPISTOLSND_sod,            // 24
	DEATHSCREAM3SND_sod,         // 25
	ATKMACHINEGUNSND_sod,        // 26
	HITENEMYSND_sod,             // 27
	SHOOTDOORSND_sod,            // 28
	DEATHSCREAM1SND_sod,         // 29
	GETMACHINESND_sod,           // 30
	GETAMMOSND_sod,              // 31
	SHOOTSND_sod,                // 32
	HEALTH1SND_sod,              // 33
	HEALTH2SND_sod,              // 34
	BONUS1SND_sod,               // 35
	BONUS2SND_sod,               // 36
	BONUS3SND_sod,               // 37
	GETGATLINGSND_sod,           // 38
	ESCPRESSEDSND_sod,           // 39
	LEVELDONESND_sod,            // 40
	DOGBARKSND_sod,              // 41
	ENDBONUS1SND_sod,            // 42
	ENDBONUS2SND_sod,            // 43
	BONUS1UPSND_sod,             // 44
	BONUS4SND_sod,               // 45
	PUSHWALLSND_sod,             // 46
	NOBONUSSND_sod,              // 47
	PERCENT100SND_sod,           // 48
	BOSSACTIVESND_sod,           // 49
	DEATHSCREAM4SND_sod,         // 50
	SCHUTZADSND_sod,             // 51
	AHHHGSND_sod,                // 52
	DEATHSCREAM5SND_sod,         // 53
	DEATHSCREAM7SND_sod,         // 54
	DEATHSCREAM8SND_sod,         // 55
	LEBENSND_sod,                // 56
	DEATHSCREAM6SND_sod,         // 57
	NAZIFIRESND_sod,             // 58
	BOSSFIRESND_sod,             // 59
	SSFIRESND_sod,               // 60
	SLURPIESND_sod,              // 61
	GHOSTFADESND_sod,            // 62
	DEATHSCREAM9SND_sod,         // 63
	GETAMMOBOXSND_sod,           // 64
	ANGELSIGHTSND_sod,           // 65
	SPIONSND_sod,                // 66
	NEINSOVASSND_sod,            // 67
	DOGATTACKSND_sod,            // 68
	ANGELFIRESND_sod,            // 69
	TRANSSIGHTSND_sod,           // 70
	TRANSDEATHSND_sod,           // 71
	WILHELMSIGHTSND_sod,         // 72
	WILHELMDEATHSND_sod,         // 73
	UBERDEATHSND_sod,            // 74
	KNIGHTSIGHTSND_sod,          // 75
	KNIGHTDEATHSND_sod,          // 76
	ANGELDEATHSND_sod,           // 77
	KNIGHTMISSILESND_sod,        // 78
	GETSPEARSND_sod,             // 79
	ANGELTIREDSND_sod,           // 80
	LASTSOUND_sod
} soundnames_sod;

//
// Base offsets
//
// IOANCH 20130202: named as symbols
#define STARTPCSOUNDS_sod		0
#define STARTADLIBSOUNDS_sod	LASTSOUND_sod
#define STARTDIGISOUNDS_sod		(2*LASTSOUND_sod)
#define STARTMUSIC_sod		    (3*LASTSOUND_sod)

//
// Music names & indexes
//
typedef enum {
	XFUNKIE_MUS_sod,             // 0
	DUNGEON_MUS_sod,             // 1
	XDEATH_MUS_sod,              // 2
	GETTHEM_MUS_sod,             // 3
	XTIPTOE_MUS_sod,             // 4
	GOINGAFT_MUS_sod,            // 5
	URAHERO_MUS_sod,             // 6
	XTHEEND_MUS_sod,             // 7
	NAZI_OMI_MUS_sod,            // 8
	POW_MUS_sod,                 // 9
	TWELFTH_MUS_sod,             // 10
	SEARCHN_MUS_sod,             // 11
	SUSPENSE_MUS_sod,            // 12
	ZEROHOUR_MUS_sod,            // 13
	WONDERIN_MUS_sod,            // 14
	ULTIMATE_MUS_sod,            // 15
	ENDLEVEL_MUS_sod,            // 16
	XEVIL_MUS_sod,               // 17
	XJAZNAZI_MUS_sod,            // 18
	COPYPRO_MUS_sod,             // 19
	XAWARD_MUS_sod,              // 20
	XPUTIT_MUS_sod,              // 21
	XGETYOU_MUS_sod,             // 22
	XTOWER2_MUS_sod,             // 23
	LASTMUSIC_sod
} musicnames_sod;

// IOANCH 20130202: named as symbols
#define NUMSOUNDS_sod		    LASTSOUND_sod
#define NUMSNDCHUNKS_sod	    (STARTMUSIC_sod + LASTMUSIC_sod)

/////////////////////////////////////////////////
//
// Thanks for playing with MUSE!
//
/////////////////////////////////////////////////
