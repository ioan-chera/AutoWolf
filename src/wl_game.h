/*
 =============================================================================
 
 WL_GAME DEFINITIONS
 
 =============================================================================
 */

#ifndef WL_GAME_H_
#define WL_GAME_H_

extern  gametype        gamestate;
extern  byte            bordercol;
extern  std::string         cfg_demoname;

namespace Game
{
enum
{
	NormalSkillOffset = 36,
	MutantSkillOffset = 18,

	GuardStandHard1 = 180,
	GuardStandHard2,
	GuardStandHard3,
	GuardStandHard4,
	GuardStandMedium1 = GuardStandHard1 - NormalSkillOffset,
	GuardStandMedium2,
	GuardStandMedium3,
	GuardStandMedium4,
	GuardStandEasy1 = GuardStandMedium1 - NormalSkillOffset,
	GuardStandEasy2,
	GuardStandEasy3,
	GuardStandEasy4,

	GuardPatrolHard1 = 184,
	GuardPatrolHard2,
	GuardPatrolHard3,
	GuardPatrolHard4,
	GuardPatrolMedium1 = GuardPatrolHard1 - NormalSkillOffset,
	GuardPatrolMedium2,
	GuardPatrolMedium3,
	GuardPatrolMedium4,
	GuardPatrolEasy1 = GuardPatrolMedium1 - NormalSkillOffset,
	GuardPatrolEasy2,
	GuardPatrolEasy3,
	GuardPatrolEasy4,

	OfficerStandHard1 = 188,
	OfficerStandHard2,
	OfficerStandHard3,
	OfficerStandHard4,
	OfficerStandMedium1 = OfficerStandHard1 - NormalSkillOffset,
	OfficerStandMedium2,
	OfficerStandMedium3,
	OfficerStandMedium4,
	OfficerStandEasy1 = OfficerStandMedium1 - NormalSkillOffset,
	OfficerStandEasy2,
	OfficerStandEasy3,
	OfficerStandEasy4,

	OfficerPatrolHard1 = 192,
	OfficerPatrolHard2,
	OfficerPatrolHard3,
	OfficerPatrolHard4,
	OfficerPatrolMedium1 = OfficerPatrolHard1 - NormalSkillOffset,
	OfficerPatrolMedium2,
	OfficerPatrolMedium3,
	OfficerPatrolMedium4,
	OfficerPatrolEasy1 = OfficerPatrolMedium1 - NormalSkillOffset,
	OfficerPatrolEasy2,
	OfficerPatrolEasy3,
	OfficerPatrolEasy4,

	SSStandHard1 = 198,
	SSStandHard2,
	SSStandHard3,
	SSStandHard4,
	SSStandMedium1 = SSStandHard1 - NormalSkillOffset,
	SSStandMedium2,
	SSStandMedium3,
	SSStandMedium4,
	SSStandEasy1 = SSStandMedium1 - NormalSkillOffset,
	SSStandEasy2,
	SSStandEasy3,
	SSStandEasy4,

	SSPatrolHard1 = 202,
	SSPatrolHard2,
	SSPatrolHard3,
	SSPatrolHard4,
	SSPatrolMedium1 = SSPatrolHard1 - NormalSkillOffset,
	SSPatrolMedium2,
	SSPatrolMedium3,
	SSPatrolMedium4,
	SSPatrolEasy1 = SSPatrolMedium1 - NormalSkillOffset,
	SSPatrolEasy2,
	SSPatrolEasy3,
	SSPatrolEasy4,

	DogStandHard1 = 206,
	DogStandHard2,
	DogStandHard3,
	DogStandHard4,
	DogStandMedium1 = DogStandHard1 - NormalSkillOffset,
	DogStandMedium2,
	DogStandMedium3,
	DogStandMedium4,
	DogStandEasy1 = DogStandMedium1 - NormalSkillOffset,
	DogStandEasy2,
	DogStandEasy3,
	DogStandEasy4,

	DogPatrolHard1 = 210,
	DogPatrolHard2,
	DogPatrolHard3,
	DogPatrolHard4,
	DogPatrolMedium1 = DogPatrolHard1 - NormalSkillOffset,
	DogPatrolMedium2,
	DogPatrolMedium3,
	DogPatrolMedium4,
	DogPatrolEasy1 = DogPatrolMedium1 - NormalSkillOffset,
	DogPatrolEasy2,
	DogPatrolEasy3,
	DogPatrolEasy4,

	MutantStandHard1 = 252,
	MutantStandHard2,
	MutantStandHard3,
	MutantStandHard4,
	MutantStandMedium1 = MutantStandHard1 - MutantSkillOffset,
	MutantStandMedium2,
	MutantStandMedium3,
	MutantStandMedium4,
	MutantStandEasy1 = MutantStandMedium1 - MutantSkillOffset,
	MutantStandEasy2,
	MutantStandEasy3,
	MutantStandEasy4,

	MutantPatrolHard1 = 256,
	MutantPatrolHard2,
	MutantPatrolHard3,
	MutantPatrolHard4,
	MutantPatrolMedium1 = MutantPatrolHard1 - MutantSkillOffset,
	MutantPatrolMedium2,
	MutantPatrolMedium3,
	MutantPatrolMedium4,
	MutantPatrolEasy1 = MutantPatrolMedium1 - MutantSkillOffset,
	MutantPatrolEasy2,
	MutantPatrolEasy3,
	MutantPatrolEasy4,

	Spectre = 106,
	BossAngel,
	BossTrans = 125,
	BossUber = 142,
	BossWill,
	FakeHitler = 160,
	BossDeath,
	BossHitler = 178,
	BossFat,
	BossSchabbs = 196,
	BossGretel,
	BossHans = 214,
	BossGift,

};
};

void    SetupGameLevel ();
void    GameLoop ();
void    DrawPlayBorder ();
void    DrawStatusBorder (byte color);
void    DrawPlayScreen ();
void    DrawPlayBorderSides ();
void    ShowActStatus();

void    PlayDemo (int demonumber);
void    RecordDemo ();

// IOANCH 20130302: unification

extern  int32_t            spearx,speary;
extern  unsigned        spearangle;
extern  Boolean8         spearflag;

// IOANCH: cleared ClearMemory
// IOANCH 20130621: added constant #defines for player starts
#define PLAYER_START_NORTH 19
#define PLAYER_START_EAST  20
#define PLAYER_START_SOUTH 21
#define PLAYER_START_WEST  22

// JAB
// IOANCH 20130802: made these as functions
#define PlaySoundLocTile(s,tx,ty)       PlaySoundLocGlobal(s,(((int32_t)(tx) << TILESHIFT) + (1L << (TILESHIFT - 1))),(((int32_t)ty << TILESHIFT) + (1L << (TILESHIFT - 1))))
//#define PlaySoundLocActor(s,ob)         PlaySoundLocGlobal(s,(ob)->x,(ob)->y)
void    PlaySoundLocActor(word s, const objtype *ob,  objtype *source = NULL);
void    PlaySoundLocGlobal(word s,fixed gx,fixed gy);
void UpdateSoundLoc();


#endif
