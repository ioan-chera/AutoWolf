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


#include "obattrib.h"

const int atr::hitpoints[][4]=
{
    {-1,   -1,   -1,   -1},     // nothing,
    {-1,   -1,   -1,   -1},     // playerobj,
    {-1,   -1,   -1,   -1},     // inertobj,
    {25,   25,   25,   25},     // guardobj,
    {50,   50,   50,   50},     // officerobj,
    {100,  100,  100,  100},    // ssobj,
    {1,    1,    1,    1,},     // dogobj,
    {850,  950,  1050, 1200},   // bossobj,
    {850,  950,  1550, 2400},   // schabbobj,
    {200,  300,  400,  500},    // fakeobj,
    {800,  950,  1050, 1200},   // mechahitlerobj,
    {45,   55,   55,   65},     // mutantobj,
    {-1,   -1,   -1,   -1},     // needleobj,
    {-1,   -1,   -1,   -1},     // fireobj,
    {-1,   -1,   -1,   -1},     // bjobj,
    {25,   25,   25,   25},     // ghostobj,
    {-1,   -1,   -1,   -1},     // realhitlerobj,
    {850,  950,  1050, 1200},   // gretelobj,
    {850,  950,  1050, 1200},   // giftobj,
    {850,  950,  1050, 1200},   // fatobj,
    {-1,   -1,   -1,   -1},     // rocketobj,
    {5,    10,   15,   25},     // spectreobj,
    {1450, 1550, 1650, 2000},   // angelobj,
    {850,  950,  1050, 1200},   // transobj,
    {1050, 1150, 1250, 1400},   // uberobj,
    {950,  1050, 1150, 1300},   // willobj,
    {1250, 1350, 1450, 1600},   // deathobj,
    {-1,   -1,   -1,   -1},     // hrocketobj,
    {-1,   -1,   -1,   -1},     // sparkobj
};

const atr::speed atr::speeds[]=
{
	{-1,        -1,         },	// nothing,
	{-1,        -1,         },	// playerobj,
	{-1,        -1,         },	// inertobj,
	{SPDPATROL, SPDPATROL*3,},	// guardobj,
	{SPDPATROL, SPDPATROL*5,},	// officerobj,
	{SPDPATROL, SPDPATROL*4,},	// ssobj,
	{SPDDOG,    SPDDOG*2,   },	// dogobj,
	{-1,        SPDPATROL*3,},	// bossobj,
	{-1,        SPDPATROL*3,},	// schabbobj,
	{-1,        SPDPATROL*3,},	// fakeobj,
	{-1,        SPDPATROL*3,},	// mechahitlerobj,
	{SPDPATROL, SPDPATROL*3,},	// mutantobj,
	{-1,        -1,         },	// needleobj,
	{-1,        -1,         },	// fireobj,
	{-1,        -1,         },	// bjobj,
	{-1,        SPDDOG*2,   },	// ghostobj,
	{-1,        SPDPATROL*5,},	// realhitlerobj,
	{-1,        SPDPATROL*3,},	// gretelobj,
	{-1,        SPDPATROL*3,},	// giftobj,
	{-1,        SPDPATROL*3,},	// fatobj,
	{-1,        -1,         },	// rocketobj,
	{-1,        800,        },	// spectreobj,
	{-1,        1536,       },	// angelobj,
	{-1,        1536,       },	// transobj,
	{-1,        3000,       },	// uberobj,
	{-1,        2048,       },	// willobj,
	{-1,        2048,       },	// deathobj,
	{-1,        -1,         },	// hrocketobj,
	{-1,        -1,         },	// sparkobj
};

const atr::sound atr::sounds[] =
{
	{-1,              -1},	// nothing,
	{-1,              -1},	// playerobj,
	{-1,              -1},	// inertobj,
	{HALTSND,         DEATHSCREAM1SND},	// guardobj,
	{SPIONSND,        NEINSOVASSND},	// officerobj,
	{SCHUTZADSND,     LEBENSND},	// ssobj,
	{DOGBARKSND,      DOGDEATHSND},	// dogobj,
	{GUTENTAGSND,     MUTTISND},	// bossobj,
	{SCHABBSHASND,    MEINGOTTSND},	// schabbobj,
	{TOT_HUNDSND,     HITLERHASND},	// fakeobj,
	{DIESND,          SCHEISTSND},	// mechahitlerobj,
	{-1,              AHHHGSND},	// mutantobj,
	{-1,              -1},	// needleobj,
	{-1,              -1},	// fireobj,
	{-1,              -1},	// bjobj,
	{-1,              -1},	// ghostobj,
	{DIESND,          EVASND},	// realhitlerobj,
	{KEINSND,         MEINSND},	// gretelobj,
	{EINESND,         DONNERSND},	// giftobj,
	{ERLAUBENSND,     ROSESND},	// fatobj,
	{-1,              -1},	// rocketobj,
	{GHOSTSIGHTSND,   GHOSTFADESND},	// spectreobj,
	{ANGELSIGHTSND,   ANGELDEATHSND},	// angelobj,
	{TRANSSIGHTSND,   TRANSDEATHSND},	// transobj,
	{-1,              UBERDEATHSND},	// uberobj,
	{WILHELMSIGHTSND, WILHELMDEATHSND},	// willobj,
	{KNIGHTSIGHTSND,  KNIGHTDEATHSND},	// deathobj,
	{-1,              -1},	// hrocketobj,
	{-1,              -1},	// sparkobj
};

const atr::state atr::states[] =
{
	{NULL,            NULL,        NULL,             NULL,       NULL,        NULL},        // nothing,
	{NULL,            NULL,        NULL,             NULL,       NULL,        NULL},        // playerobj,
	{NULL,            NULL,        NULL,             NULL,       NULL,        NULL},        // inertobj,
	{&s_grdstand,     &s_grdpath1, &s_grdchase1,     &s_grdpain, &s_grdpain1, &s_grdshoot1, &s_grddie1}, // guardobj,
	{&s_ofcstand,     &s_ofcpath1, &s_ofcchase1,     &s_ofcpain, &s_ofcpain1, &s_ofcshoot1, &s_ofcdie1}, // officerobj,
	{&s_ssstand,      &s_sspath1,  &s_sschase1,      &s_sspain,  &s_sspain1,  &s_ssshoot1, &s_ssdie1},  // ssobj,
	{NULL,            &s_dogpath1, &s_dogchase1,     NULL,       NULL,        NULL, &s_dogdie1},        // dogobj,
	{&s_bossstand,    NULL,        &s_bosschase1,    NULL,       NULL,        &s_bossshoot1, &s_bossdie1},        // bossobj,
	{&s_schabbstand,  NULL,        &s_schabbchase1,  NULL,       NULL,        NULL, &s_schabbdie1},        // schabbobj,
	{&s_fakestand,    NULL,        &s_fakechase1,    NULL,       NULL,        NULL, &s_fakedie1},        // fakeobj,
	{&s_mechastand,   NULL,        &s_mechachase1,   NULL,       NULL,        &s_mechashoot1, &s_mechadie1},        // mechahitlerobj,
	{&s_mutstand,     &s_mutpath1, &s_mutchase1,     &s_mutpain, &s_mutpain1, &s_mutshoot1, &s_mutdie1}, // mutantobj,
	{NULL,            NULL,        NULL,             NULL,       NULL,        NULL},        // needleobj,
	{NULL,            NULL,        NULL,             NULL,       NULL,        NULL},        // fireobj,
	{NULL,            NULL,        NULL,             NULL,       NULL,        NULL},        // bjobj,
	{NULL,            NULL,        &s_blinkychase1,  NULL,       NULL,        NULL},        // ghostobj,
	{NULL,            NULL,        &s_hitlerchase1,  NULL,       NULL,        &s_hitlershoot1, &s_hitlerdie1},        // realhitlerobj,
	{&s_gretelstand,  NULL,        &s_gretelchase1,  NULL,       NULL,        &s_gretelshoot1, &s_greteldie1},        // gretelobj,
	{&s_giftstand,    NULL,        &s_giftchase1,    NULL,       NULL,        NULL, &s_giftdie1},        // giftobj,
	{&s_fatstand,     NULL,        &s_fatchase1,     NULL,       NULL,        NULL, &s_fatdie1},        // fatobj,
	{NULL,            NULL,        NULL,             NULL,       NULL,        NULL},        // rocketobj,
	{&s_spectrewait1, NULL,        &s_spectrechase1, NULL,       NULL,        NULL, &s_spectredie1},        // spectreobj,
	{&s_angelstand,   NULL,        &s_angelchase1,   NULL,       NULL,        &s_angelshoot1, &s_angeldie1},        // angelobj,
	{&s_transstand,   NULL,        &s_transchase1,   NULL,       NULL,        &s_transshoot1, &s_transdie0},        // transobj,
	{&s_uberstand,    NULL,        &s_uberchase1,    NULL,       NULL,        &s_ubershoot1, &s_uberdie0},        // uberobj,
	{&s_willstand,    NULL,        &s_willchase1,    NULL,       NULL,        &s_willshoot1, &s_willdie1},        // willobj,
	{&s_deathstand,   NULL,        &s_deathchase1,   NULL,       NULL,        &s_deathshoot1, &s_deathdie1},        // deathobj,
	{NULL,            NULL,        NULL,             NULL,       NULL,        NULL},        // hrocketobj,
	{NULL,            NULL,        NULL,             NULL,       NULL,        NULL},        // sparkobj
};

const atr::action atr::actions[] =
{
	{LAM{}, LAM{}},	// nothing,
	{LAM{}, LAM{}},	// playerobj,
	{LAM{}, LAM{}},	// inertobj,
	{LAM{}, LAM{}},	// guardobj,
	{LAM{}, LAM{}},	// officerobj,
	{LAM{}, LAM{}},	// ssobj,
	{LAM{}, LAM{}},	// dogobj,
	{LAM{
        newobj->speed = SPDPATROL;
        newobj->dir = nodir;
    }, LAM{}},	// bossobj,
	{LAM{
		newobj->speed = SPDPATROL;
        newobj->dir = nodir;
		if (DigiMode != sds_Off)
			s_schabbdie2.tictime = 140;
		else
			s_schabbdie2.tictime = 5;
	}, 
        LAM{}},	// schabbobj,
	{LAM{
        newobj->speed = SPDPATROL;
        newobj->dir = nodir;
    }, LAM{}},	// fakeobj,
	{LAM{
		newobj->speed = SPDPATROL;
        newobj->dir = nodir;
		if (DigiMode != sds_Off)
			s_hitlerdie2.tictime = 140;
		else
			s_hitlerdie2.tictime = 5;
	}, 
        LAM{}},	// mechahitlerobj,
	{LAM{}, LAM{}},	// mutantobj,
	{LAM{}, LAM{}},	// needleobj,
	{LAM{}, LAM{}},	// fireobj,
	{LAM{}, LAM{}},	// bjobj,
	{LAM{}, LAM{}},	// ghostobj,
	{LAM{}, LAM{}},	// realhitlerobj,
	{LAM{
        newobj->speed = SPDPATROL;
        newobj->dir = nodir;
    }, LAM{}},	// gretelobj,
	{LAM{
        newobj->speed = SPDPATROL;
        newobj->dir = nodir;
		if (DigiMode != sds_Off)
			s_giftdie2.tictime = 140;
		else
			s_giftdie2.tictime = 5;
    }, LAM{}},	// giftobj,
	{LAM{
        newobj->speed = SPDPATROL;
        newobj->dir = nodir;
        if (DigiMode != sds_Off)
            s_fatdie2.tictime = 140;
        else
            s_fatdie2.tictime = 5;
    }, LAM{}},	// fatobj,
	{LAM{}, LAM{}},	// rocketobj,
	{LAM{
        newobj->flags |= FL_BONUS;
    }, LAM{}},	// spectreobj,
	{LAM{
		if (SoundBlasterPresent && DigiMode != sds_Off)
			s_angeldie11.tictime = 105;
	}, 
        LAM{}},	// angelobj,
	{LAM{
		if (SoundBlasterPresent && DigiMode != sds_Off)
			s_transdie01.tictime = 105;
	}, 
        LAM{}},	// transobj,
	{LAM{
		if (SoundBlasterPresent && DigiMode != sds_Off)
			s_uberdie01.tictime = 70;
	}, 
        LAM{}},	// uberobj,
	{LAM{
		if (SoundBlasterPresent && DigiMode != sds_Off)
			s_willdie2.tictime = 70;
	}, 
        LAM{}},	// willobj,
	{LAM{
		if (SoundBlasterPresent && DigiMode != sds_Off)
			s_deathdie2.tictime = 105;
	}, 
        LAM{}},	// deathobj,
	{LAM{}, LAM{}},	// hrocketobj,
	{LAM{}, LAM{}},	// sparkobj
};

const int atr::threatrange[] =
{
    -1,     // nothing,
    -1,     // playerobj,
    -1,     // inertobj,
    12,     // guardobj,
    14,     // officerobj,
    16,     // ssobj,
    -1,     // dogobj,
    16,     // bossobj,
    -1,     // schabbobj,
    -1,     // fakeobj,
    16,     // mechahitlerobj,
    14,     // mutantobj, !!!NEARBY_THREAT
    -1,     // needleobj,
    -1,     // fireobj,
    -1,     // bjobj,
    -1,     // ghostobj,   !!!NEARBY_THREAT
    16,     // realhitlerobj,
    16,     // gretelobj,
    -1,     // giftobj,
    16,     // fatobj,
    -1,     // rocketobj,
    -1,     // spectreobj,
    -1,     // angelobj,
    16,     // transobj,
    16,     // uberobj,
    16,     // willobj,
    16,     // deathobj,
    -1,     // hrocketobj,
    -1,     // sparkobj
};

const uint64_t atr::flags[] =
{
    0,      // nothing,
    0,      // playerobj,
    0,      // inertobj,
    0,      // guardobj,
    0,      // officerobj,
    0,      // ssobj,
    0,      // dogobj,
    ATR_BOSS_SOUNDS,      // bossobj,
    ATR_BOSS_SOUNDS,      // schabbobj,
    ATR_BOSS_SOUNDS,      // fakeobj,
    ATR_BOSS_SOUNDS,      // mechahitlerobj,
    ATR_NEARBY_THREAT,      // mutantobj, !!!NEARBY_THREAT
    0,      // needleobj,
    0,      // fireobj,
    0,      // bjobj,
    ATR_NEARBY_THREAT,      // ghostobj,   !!!NEARBY_THREAT
    ATR_BOSS_SOUNDS,      // realhitlerobj,
    ATR_BOSS_SOUNDS,      // gretelobj,
    ATR_BOSS_SOUNDS,      // giftobj,
    ATR_BOSS_SOUNDS,      // fatobj,
    0,      // rocketobj,
    ATR_BOSS_SOUNDS,      // spectreobj,
    ATR_BOSS_SOUNDS,      // angelobj,
    ATR_BOSS_SOUNDS,      // transobj,
    ATR_BOSS_SOUNDS,      // uberobj,
    ATR_BOSS_SOUNDS,      // willobj,
    ATR_BOSS_SOUNDS,      // deathobj,
    0,      // hrocketobj,
    0,      // sparkobj
};

const int32_t atr::points[] =
{
    -1,                 // nothing,
    -1,                 // playerobj,
    -1,                 // inertobj,
    I_GUARDSCORE,       // guardobj,
    I_OFFICERSCORE,     // officerobj,
    I_SSSCORE,          // ssobj,
    I_DOGSCORE,         // dogobj,
    I_BOSSSCORE,        // bossobj,
    I_BOSSSCORE,        // schabbobj,
    I_FAKEHITLERSCORE,  // fakeobj,
    I_BOSSSCORE,        // mechahitlerobj,
    I_MUTANTSCORE,      // mutantobj,
    -1,                 // needleobj,
    -1,                 // fireobj,
    -1,                 // bjobj,
    -1,                 // ghostobj,
    I_BOSSSCORE,        // realhitlerobj,
    I_BOSSSCORE,        // gretelobj,
    I_BOSSSCORE,        // giftobj,
    I_BOSSSCORE,        // fatobj,
    -1,                 // rocketobj,
    I_SPECTRESCORE,     // spectreobj,
    I_BOSSSCORE,        // angelobj,
    I_BOSSSCORE,        // transobj,
    I_BOSSSCORE,        // uberobj,
    I_BOSSSCORE,        // willobj,
    I_BOSSSCORE,        // deathobj,
    -1,                 // hrocketobj,
    -1,                 // sparkobj
};