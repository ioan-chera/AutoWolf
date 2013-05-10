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


#include "SODFlag.h"

SODFlag SPEAR;

const unsigned int SODFlag::gfxvmap[][2] =
{
    {3,                     BJCOLLAPSE1PIC_sod},
    {3,                     BJCOLLAPSE2PIC_sod},
    {3,                     BJCOLLAPSE3PIC_sod},
    {3,                     BJCOLLAPSE4PIC_sod},
    {3,                     BJOUCHPIC_sod},
    {3,                     BJWAITING1PIC_sod},
    {3,                     BJWAITING2PIC_sod},
    {3,                     BOSSPIC1PIC_sod},
    {3,                     BOSSPIC2PIC_sod},
    {3,                     BOSSPIC3PIC_sod},
    {3,                     BOSSPIC4PIC_sod},
    {C_BABYMODEPIC_wl6,     C_BABYMODEPIC_sod},
    {3,                     C_BACKDROPPIC_sod},
    {C_CODEPIC_wl6,         3},
    {C_CONTROLPIC_wl6,      C_CONTROLPIC_sod},
    {C_CURSOR1PIC_wl6,      C_CURSOR1PIC_sod},
    {C_CURSOR2PIC_wl6,      C_CURSOR2PIC_sod},
    {C_CUSTOMIZEPIC_wl6,    C_CUSTOMIZEPIC_sod},
    {C_DIGITITLEPIC_wl6,    C_DIGITITLEPIC_sod},
    {C_DISKLOADING1PIC_wl6, C_DISKLOADING1PIC_sod},
    {C_DISKLOADING2PIC_wl6, C_DISKLOADING2PIC_sod},
    {C_EASYPIC_wl6,         C_EASYPIC_sod},
    {C_EPISODE1PIC_wl6,     3},
    {C_EPISODE2PIC_wl6,     3},
    {C_EPISODE3PIC_wl6,     3},
    {C_EPISODE4PIC_wl6,     3},
    {C_EPISODE5PIC_wl6,     3},
    {C_EPISODE6PIC_wl6,     3},
    {C_FXTITLEPIC_wl6,      C_FXTITLEPIC_sod},
    {C_HARDPIC_wl6,         C_HARDPIC_sod},
    {3,                     C_HOWTOUGHPIC_sod},
    {C_JOY1PIC_wl6,         C_JOY1PIC_sod},
    {C_JOY2PIC_wl6,         C_JOY2PIC_sod},
    {3,                     C_JOYSTICKPIC_sod},
    {3,                     C_KEYBOARDPIC_sod},
    {C_LEVELPIC_wl6,        3},
    {C_LOADGAMEPIC_wl6,     C_LOADGAMEPIC_sod},
    {C_LOADSAVEDISKPIC_wl6, 3},
    {C_MOUSELBACKPIC_wl6,   C_MOUSELBACKPIC_sod},
    {3,                     C_MOUSEPIC_sod},
    {C_MUSICTITLEPIC_wl6,   C_MUSICTITLEPIC_sod},
    {C_NAMEPIC_wl6,         3},
    {C_NORMALPIC_wl6,       C_NORMALPIC_sod},
    {C_NOTSELECTEDPIC_wl6,  C_NOTSELECTEDPIC_sod},
    {C_OPTIONSPIC_wl6,      C_OPTIONSPIC_sod},
    {C_SAVEGAMEPIC_wl6,     C_SAVEGAMEPIC_sod},
    {C_SCOREPIC_wl6,        3},
    {C_SELECTEDPIC_wl6,     C_SELECTEDPIC_sod},
    {C_TIMECODEPIC_wl6,     3},
    {3,                     C_WONSPEARPIC_sod},
    {3,                     COPYPROTBOXPIC_sod},
    {3,                     COPYPROTTOPPIC_sod},
    {CREDITSPIC_wl6,        CREDITSPIC_sod},
    {3,                     END1PALETTE_sod},
    {3,                     END2PALETTE_sod},
    {3,                     END3PALETTE_sod},
    {3,                     END4PALETTE_sod},
    {3,                     END5PALETTE_sod},
    {3,                     END6PALETTE_sod},
    {3,                     END7PALETTE_sod},
    {3,                     END8PALETTE_sod},
    {3,                     END9PALETTE_sod},
    {3,                     ENDPICPIC_sod},
    {3,                     ENDSCREEN11PIC_sod},
    {3,                     ENDSCREEN12PIC_sod},
    {3,                     ENDSCREEN3PIC_sod},
    {3,                     ENDSCREEN4PIC_sod},
    {3,                     ENDSCREEN5PIC_sod},
    {3,                     ENDSCREEN6PIC_sod},
    {3,                     ENDSCREEN7PIC_sod},
    {3,                     ENDSCREEN8PIC_sod},
    {3,                     ENDSCREEN9PIC_sod},
    {ERRORSCREEN_wl6,       ERRORSCREEN_sod},
    {FACE1APIC_wl6,         FACE1APIC_sod},
    {FACE1BPIC_wl6,         FACE1BPIC_sod},
    {FACE1CPIC_wl6,         FACE1CPIC_sod},
    {FACE2APIC_wl6,         FACE2APIC_sod},
    {FACE2BPIC_wl6,         FACE2BPIC_sod},
    {FACE2CPIC_wl6,         FACE2CPIC_sod},
    {FACE3APIC_wl6,         FACE3APIC_sod},
    {FACE3BPIC_wl6,         FACE3BPIC_sod},
    {FACE3CPIC_wl6,         FACE3CPIC_sod},
    {FACE4APIC_wl6,         FACE4APIC_sod},
    {FACE4BPIC_wl6,         FACE4BPIC_sod},
    {FACE4CPIC_wl6,         FACE4CPIC_sod},
    {FACE5APIC_wl6,         FACE5APIC_sod},
    {FACE5BPIC_wl6,         FACE5BPIC_sod},
    {FACE5CPIC_wl6,         FACE5CPIC_sod},
    {FACE6APIC_wl6,         FACE6APIC_sod},
    {FACE6BPIC_wl6,         FACE6BPIC_sod},
    {FACE6CPIC_wl6,         FACE6CPIC_sod},
    {FACE7APIC_wl6,         FACE7APIC_sod},
    {FACE7BPIC_wl6,         FACE7BPIC_sod},
    {FACE7CPIC_wl6,         FACE7CPIC_sod},
    {FACE8APIC_wl6,         FACE8APIC_sod},
    {GATLINGGUNPIC_wl6,     GATLINGGUNPIC_sod},
    {GETPSYCHEDPIC_wl6,     GETPSYCHEDPIC_sod},
    {3,                     GODMODEFACE1PIC_sod},
    {3,                     GODMODEFACE2PIC_sod},
    {3,                     GODMODEFACE3PIC_sod},
    {GOLDKEYPIC_wl6,        GOLDKEYPIC_sod},
    {GOTGATLINGPIC_wl6,     GOTGATLINGPIC_sod},
    {GUNPIC_wl6,            GUNPIC_sod},
    {H_BJPIC_wl6,           3},
    {H_BLAZEPIC_wl6,        3},
    {H_BOTTOMINFOPIC_wl6,   3},
    {H_CASTLEPIC_wl6,       3},
    {H_LEFTWINDOWPIC_wl6,   3},
    {H_RIGHTWINDOWPIC_wl6,  3},
    {H_TOPWINDOWPIC_wl6,    3},
    {HIGHSCORESPIC_wl6,     HIGHSCORESPIC_sod},
    {3,                     IDGUYS1PIC_sod},
    {3,                     IDGUYS2PIC_sod},
    {3,                     IDGUYSPALETTE_sod},
    {KNIFEPIC_wl6,          KNIFEPIC_sod},
    {L_APIC_wl6,            L_APIC_sod},
    {L_APOSTROPHEPIC_wl6,   L_APOSTROPHEPIC_sod},
    {L_BJWINSPIC_wl6,       L_BJWINSPIC_sod},
    {L_BPIC_wl6,            L_BPIC_sod},
    {L_COLONPIC_wl6,        L_COLONPIC_sod},
    {L_CPIC_wl6,            L_CPIC_sod},
    {L_DPIC_wl6,            L_DPIC_sod},
    {L_EPIC_wl6,            L_EPIC_sod},
    {L_EXPOINTPIC_wl6,      L_EXPOINTPIC_sod},
    {L_FPIC_wl6,            L_FPIC_sod},
    {L_GPIC_wl6,            L_GPIC_sod},
    {L_GUY2PIC_wl6,         L_GUY2PIC_sod},
    {L_GUYPIC_wl6,          L_GUYPIC_sod},
    {L_HPIC_wl6,            L_HPIC_sod},
    {L_IPIC_wl6,            L_IPIC_sod},
    {L_JPIC_wl6,            L_JPIC_sod},
    {L_KPIC_wl6,            L_KPIC_sod},
    {L_LPIC_wl6,            L_LPIC_sod},
    {L_MPIC_wl6,            L_MPIC_sod},
    {L_NPIC_wl6,            L_NPIC_sod},
    {L_NUM0PIC_wl6,         L_NUM0PIC_sod},
    {L_NUM1PIC_wl6,         L_NUM1PIC_sod},
    {L_NUM2PIC_wl6,         L_NUM2PIC_sod},
    {L_NUM3PIC_wl6,         L_NUM3PIC_sod},
    {L_NUM4PIC_wl6,         L_NUM4PIC_sod},
    {L_NUM5PIC_wl6,         L_NUM5PIC_sod},
    {L_NUM6PIC_wl6,         L_NUM6PIC_sod},
    {L_NUM7PIC_wl6,         L_NUM7PIC_sod},
    {L_NUM8PIC_wl6,         L_NUM8PIC_sod},
    {L_NUM9PIC_wl6,         L_NUM9PIC_sod},
    {L_OPIC_wl6,            L_OPIC_sod},
    {L_PERCENTPIC_wl6,      L_PERCENTPIC_sod},
    {L_PPIC_wl6,            L_PPIC_sod},
    {L_QPIC_wl6,            L_QPIC_sod},
    {L_RPIC_wl6,            L_RPIC_sod},
    {L_SPIC_wl6,            L_SPIC_sod},
    {L_TPIC_wl6,            L_TPIC_sod},
    {L_UPIC_wl6,            L_UPIC_sod},
    {L_VPIC_wl6,            L_VPIC_sod},
    {L_WPIC_wl6,            L_WPIC_sod},
    {L_XPIC_wl6,            L_XPIC_sod},
    {L_YPIC_wl6,            L_YPIC_sod},
    {L_ZPIC_wl6,            L_ZPIC_sod},
    {MACHINEGUNPIC_wl6,     MACHINEGUNPIC_sod},
    {MUTANTBJPIC_wl6,       3},
    {N_0PIC_wl6,            N_0PIC_sod},
    {N_1PIC_wl6,            N_1PIC_sod},
    {N_2PIC_wl6,            N_2PIC_sod},
    {N_3PIC_wl6,            N_3PIC_sod},
    {N_4PIC_wl6,            N_4PIC_sod},
    {N_5PIC_wl6,            N_5PIC_sod},
    {N_6PIC_wl6,            N_6PIC_sod},
    {N_7PIC_wl6,            N_7PIC_sod},
    {N_8PIC_wl6,            N_8PIC_sod},
    {N_9PIC_wl6,            N_9PIC_sod},
    {N_BLANKPIC_wl6,        N_BLANKPIC_sod},
    {NOKEYPIC_wl6,          NOKEYPIC_sod},
    {ORDERSCREEN_wl6,       ORDERSCREEN_sod},
    {PAUSEDPIC_wl6,         PAUSEDPIC_sod},
    {PG13PIC_wl6,           PG13PIC_sod},
    {SILVERKEYPIC_wl6,      SILVERKEYPIC_sod},
    {STATUSBARPIC_wl6,      STATUSBARPIC_sod},
    {T_DEMO0_wl6,           T_DEMO0_sod},
    {T_DEMO1_wl6,           T_DEMO1_sod},
    {T_DEMO2_wl6,           T_DEMO2_sod},
    {T_DEMO3_wl6,           T_DEMO3_sod},
    {T_ENDART1_wl6,         T_ENDART1_sod},
    {T_ENDART2_wl6,         3},
    {T_ENDART3_wl6,         3},
    {T_ENDART4_wl6,         3},
    {T_ENDART5_wl6,         3},
    {T_ENDART6_wl6,         3},
    {T_HELPART_wl6,         3},
    {3,                     TILE8_sod},
    {3,                     TITLE1PIC_sod},
    {3,                     TITLE2PIC_sod},
    {3,                     TITLEPALETTE_sod},
    {TITLEPIC_wl6,          3},
    
    {3,BACKDROP_LUMP_END_sod},
    {3,BACKDROP_LUMP_START_sod},
    {3,CONTROL_LUMP_END_sod},
    {3,CONTROL_LUMP_START_sod},
    {CONTROLS_LUMP_END_wl6,3},
    {CONTROLS_LUMP_START_wl6,3},
    {3,COPYPROT_LUMP_END_sod},
    {3,COPYPROT_LUMP_START_sod},
    {3,EASTEREGG_LUMP_END_sod},
    {3,EASTEREGG_LUMP_START_sod},
    {3,ENDGAME1_LUMP_END_sod},
    {3,ENDGAME1_LUMP_START_sod},
    {3,ENDGAME2_LUMP_END_sod},
    {3,ENDGAME2_LUMP_START_sod},
    {3,ENDGAME_LUMP_END_sod},
    {3,ENDGAME_LUMP_START_sod},
    {3,HIGHSCORES_LUMP_END_sod},
    {3,HIGHSCORES_LUMP_START_sod},
    {LATCHPICS_LUMP_END_wl6,LATCHPICS_LUMP_END_sod},
    {LATCHPICS_LUMP_START_wl6,LATCHPICS_LUMP_START_sod},
    {LEVELEND_LUMP_END_wl6,LEVELEND_LUMP_END_sod},
    {LEVELEND_LUMP_START_wl6,LEVELEND_LUMP_START_sod},
    {3,LOADSAVE_LUMP_END_sod},
    {3,LOADSAVE_LUMP_START_sod},
    {3,NEWGAME_LUMP_END_sod},
    {3,NEWGAME_LUMP_START_sod},
    {NUMCHUNKS_wl6,NUMCHUNKS_sod},
    {NUMEXTERNS_wl6,NUMEXTERNS_sod},
    {NUMFONT_wl6,NUMFONT_sod},
    {NUMFONTM_wl6,NUMFONTM_sod},
    {NUMPICM_wl6,NUMPICM_sod},
    {NUMPICS_wl6,NUMPICS_sod},
    {NUMSPRITES_wl6,NUMSPRITES_sod},
    {NUMTILE16_wl6,NUMTILE16_sod},
    {NUMTILE16M_wl6,NUMTILE16M_sod},
    {NUMTILE32_wl6,NUMTILE32_sod},
    {NUMTILE32M_wl6,NUMTILE32M_sod},
    {NUMTILE8_wl6,NUMTILE8_sod},
    {NUMTILE8M_wl6,NUMTILE8M_sod},
    {3,OPTIONS_LUMP_END_sod},
    {3,OPTIONS_LUMP_START_sod},
    {README_LUMP_END_wl6,3},
    {README_LUMP_START_wl6,3},
    {3,SOUND_LUMP_END_sod},
    {3,SOUND_LUMP_START_sod},
    {STARTEXTERNS_wl6,STARTEXTERNS_sod},
    {STARTFONT_wl6,STARTFONT_sod},
    {STARTFONTM_wl6,STARTFONTM_sod},
    {STARTPICM_wl6,STARTPICM_sod},
    {STARTPICS_wl6,STARTPICS_sod},
    {STARTSPRITES_wl6,STARTSPRITES_sod},
    {STARTTILE16_wl6,STARTTILE16_sod},
    {STARTTILE16M_wl6,STARTTILE16M_sod},
    {STARTTILE32_wl6,STARTTILE32_sod},
    {STARTTILE32M_wl6,STARTTILE32M_sod},
    {STARTTILE8_wl6,STARTTILE8_sod},
    {STARTTILE8M_wl6,STARTTILE8M_sod},
    {STRUCTPIC_wl6,STRUCTPIC_sod},
    {3,TITLESCREEN_LUMP_END_sod},
    {3,TITLESCREEN_LUMP_START_sod},
};


void SODFlag::Initialize(const PString &basePath)
{
    FILE *f;
    
    f = fopen(basePath.withSubpath("VSWAP.SD3").buffer(), "rb");
    if(!f)
    {
        f = fopen(basePath.withSubpath("VSWAP.SD2").buffer(), "rb");
        if(!f)
        {
            f = fopen(basePath.withSubpath("VSWAP.SD1").buffer(), "rb");
            if(!f)
            {
                f = fopen(basePath.withSubpath("VSWAP.SOD").buffer(), "rb");
                if(!f)
                {
                    this->flag = false;
					return;	// none found: assume Wolf3D
                }
            }
        }
    }
    fclose(f);
	// One of the ifs failed - fall here and return SPEAR() 1
    this->flag = true;
}