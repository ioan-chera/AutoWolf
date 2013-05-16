//
// Copyright (C) 1991-1992  id Software
// Copyright (C) 2007-2011  Moritz Kroll
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


//////////////////////////////////////
//
// Graphics .H file for .SOD
// IGRAB-ed on Thu Oct 08 20:38:29 1992
//
//////////////////////////////////////
// IOANCH 20130302: unification
#ifndef GFXV_SOD_H_
#define GFXV_SOD_H_
typedef enum {
    // Lump Start
    C_BACKDROPPIC_sod=3,
    C_MOUSELBACKPIC_sod,                     // 4
    C_CURSOR1PIC_sod,                        // 5
    C_CURSOR2PIC_sod,                        // 6
    C_NOTSELECTEDPIC_sod,                    // 7
    C_SELECTEDPIC_sod,                       // 8
    // Lump Start
    C_CUSTOMIZEPIC_sod,                      // 9
    C_JOY1PIC_sod,                           // 10
    C_JOY2PIC_sod,                           // 11
    C_MOUSEPIC_sod,                          // 12
    C_JOYSTICKPIC_sod,                       // 13
    C_KEYBOARDPIC_sod,                       // 14
    C_CONTROLPIC_sod,                        // 15
    // Lump Start
    C_OPTIONSPIC_sod,                        // 16
    // Lump Start
    C_FXTITLEPIC_sod,                        // 17
    C_DIGITITLEPIC_sod,                      // 18
    C_MUSICTITLEPIC_sod,                     // 19
    // Lump Start
    C_HOWTOUGHPIC_sod,                       // 20
    C_BABYMODEPIC_sod,                       // 21
    C_EASYPIC_sod,                           // 22
    C_NORMALPIC_sod,                         // 23
    C_HARDPIC_sod,                           // 24
    // Lump Start
    C_DISKLOADING1PIC_sod,                   // 25
    C_DISKLOADING2PIC_sod,                   // 26
    C_LOADGAMEPIC_sod,                       // 27
    C_SAVEGAMEPIC_sod,                       // 28
    // Lump Start
    HIGHSCORESPIC_sod,                       // 29
    C_WONSPEARPIC_sod,                       // 30
    // IOANCH 20130301: unification culling
    // Lump Start
    BJCOLLAPSE1PIC_sod,                      // 31
    BJCOLLAPSE2PIC_sod,                      // 32
    BJCOLLAPSE3PIC_sod,                      // 33
    BJCOLLAPSE4PIC_sod,                      // 34
    ENDPICPIC_sod,                           // 35
    // Lump Start
    L_GUYPIC_sod,                            // 36
    L_COLONPIC_sod,                          // 37
    L_NUM0PIC_sod,                           // 38
    L_NUM1PIC_sod,                           // 39
    L_NUM2PIC_sod,                           // 40
    L_NUM3PIC_sod,                           // 41
    L_NUM4PIC_sod,                           // 42
    L_NUM5PIC_sod,                           // 43
    L_NUM6PIC_sod,                           // 44
    L_NUM7PIC_sod,                           // 45
    L_NUM8PIC_sod,                           // 46
    L_NUM9PIC_sod,                           // 47
    L_PERCENTPIC_sod,                        // 48
    L_APIC_sod,                              // 49
    L_BPIC_sod,                              // 50
    L_CPIC_sod,                              // 51
    L_DPIC_sod,                              // 52
    L_EPIC_sod,                              // 53
    L_FPIC_sod,                              // 54
    L_GPIC_sod,                              // 55
    L_HPIC_sod,                              // 56
    L_IPIC_sod,                              // 57
    L_JPIC_sod,                              // 58
    L_KPIC_sod,                              // 59
    L_LPIC_sod,                              // 60
    L_MPIC_sod,                              // 61
    L_NPIC_sod,                              // 62
    L_OPIC_sod,                              // 63
    L_PPIC_sod,                              // 64
    L_QPIC_sod,                              // 65
    L_RPIC_sod,                              // 66
    L_SPIC_sod,                              // 67
    L_TPIC_sod,                              // 68
    L_UPIC_sod,                              // 69
    L_VPIC_sod,                              // 70
    L_WPIC_sod,                              // 71
    L_XPIC_sod,                              // 72
    L_YPIC_sod,                              // 73
    L_ZPIC_sod,                              // 74
    L_EXPOINTPIC_sod,                        // 75
    L_APOSTROPHEPIC_sod,                     // 76
    L_GUY2PIC_sod,                           // 77
    L_BJWINSPIC_sod,                         // 78
    // Lump Start
    TITLE1PIC_sod,                           // 79
    TITLE2PIC_sod,                           // 80
// IOANCH 20130301: unification culling

    // Lump Start
    ENDSCREEN11PIC_sod,                      // 81
    // Lump Start
    ENDSCREEN12PIC_sod,                      // 82
    ENDSCREEN3PIC_sod,                       // 83
    ENDSCREEN4PIC_sod,                       // 84
    ENDSCREEN5PIC_sod,                       // 85
    ENDSCREEN6PIC_sod,                       // 86
    ENDSCREEN7PIC_sod,                       // 87
    ENDSCREEN8PIC_sod,                       // 88
    ENDSCREEN9PIC_sod,                       // 89

    STATUSBARPIC_sod,                        // 90
    PG13PIC_sod,                             // 91
    CREDITSPIC_sod,                          // 92
// IOANCH 20130301: unification culling

    // Lump Start
    IDGUYS1PIC_sod,                          // 93
    IDGUYS2PIC_sod,                          // 94
    // Lump Start
    COPYPROTTOPPIC_sod,                      // 95
    COPYPROTBOXPIC_sod,                      // 96
    BOSSPIC1PIC_sod,                         // 97
    BOSSPIC2PIC_sod,                         // 98
    BOSSPIC3PIC_sod,                         // 99
    BOSSPIC4PIC_sod,                         // 100

    // Lump Start
    KNIFEPIC_sod,                            // 101
    GUNPIC_sod,                              // 102
    MACHINEGUNPIC_sod,                       // 103
    GATLINGGUNPIC_sod,                       // 104
    NOKEYPIC_sod,                            // 105
    GOLDKEYPIC_sod,                          // 106
    SILVERKEYPIC_sod,                        // 107
    N_BLANKPIC_sod,                          // 108
    N_0PIC_sod,                              // 109
    N_1PIC_sod,                              // 110
    N_2PIC_sod,                              // 111
    N_3PIC_sod,                              // 112
    N_4PIC_sod,                              // 113
    N_5PIC_sod,                              // 114
    N_6PIC_sod,                              // 115
    N_7PIC_sod,                              // 116
    N_8PIC_sod,                              // 117
    N_9PIC_sod,                              // 118
    FACE1APIC_sod,                           // 119
    FACE1BPIC_sod,                           // 120
    FACE1CPIC_sod,                           // 121
    FACE2APIC_sod,                           // 122
    FACE2BPIC_sod,                           // 123
    FACE2CPIC_sod,                           // 124
    FACE3APIC_sod,                           // 125
    FACE3BPIC_sod,                           // 126
    FACE3CPIC_sod,                           // 127
    FACE4APIC_sod,                           // 128
    FACE4BPIC_sod,                           // 129
    FACE4CPIC_sod,                           // 130
    FACE5APIC_sod,                           // 131
    FACE5BPIC_sod,                           // 132
    FACE5CPIC_sod,                           // 133
    FACE6APIC_sod,                           // 134
    FACE6BPIC_sod,                           // 135
    FACE6CPIC_sod,                           // 136
    FACE7APIC_sod,                           // 137
    FACE7BPIC_sod,                           // 138
    FACE7CPIC_sod,                           // 139
    FACE8APIC_sod,                           // 140
    GOTGATLINGPIC_sod,                       // 141
    GODMODEFACE1PIC_sod,                     // 142
    GODMODEFACE2PIC_sod,                     // 143
    GODMODEFACE3PIC_sod,                     // 144
    BJWAITING1PIC_sod,                       // 145
    BJWAITING2PIC_sod,                       // 146
    BJOUCHPIC_sod,                           // 147
    PAUSEDPIC_sod,                           // 148
    GETPSYCHEDPIC_sod,                       // 149

    TILE8_sod,                               // 150

    ORDERSCREEN_sod,                         // 151
    ERRORSCREEN_sod,                         // 152
    TITLEPALETTE_sod,                        // 153
    // IOANCH 20130301: unification culling

    END1PALETTE_sod,                         // 154
    END2PALETTE_sod,                         // 155
    END3PALETTE_sod,                         // 156
    END4PALETTE_sod,                         // 157
    END5PALETTE_sod,                         // 158
    END6PALETTE_sod,                         // 159
    END7PALETTE_sod,                         // 160
    END8PALETTE_sod,                         // 161
    END9PALETTE_sod,                         // 162
    IDGUYSPALETTE_sod,                       // 163

    T_DEMO0_sod,                             // 164
// IOANCH 20130301: unification culling
    T_DEMO1_sod,                             // 165
    T_DEMO2_sod,                             // 166
    T_DEMO3_sod,                             // 167
    T_ENDART1_sod,                           // 168

    ENUMEND_sod
} graphicnums_sod;

//
// Data LUMPs
//
#define BACKDROP_LUMP_START_sod		3
#define BACKDROP_LUMP_END_sod		8

#define CONTROL_LUMP_START_sod		9
#define CONTROL_LUMP_END_sod		15

#define OPTIONS_LUMP_START_sod		16
#define OPTIONS_LUMP_END_sod		16

#define SOUND_LUMP_START_sod		17
#define SOUND_LUMP_END_sod			19

#define NEWGAME_LUMP_START_sod		20
#define NEWGAME_LUMP_END_sod		24

#define LOADSAVE_LUMP_START_sod		25
#define LOADSAVE_LUMP_END_sod		28

#define HIGHSCORES_LUMP_START_sod	29
#define HIGHSCORES_LUMP_END_sod		30

#define ENDGAME_LUMP_START_sod		31
#define ENDGAME_LUMP_END_sod		35

#define LEVELEND_LUMP_START_sod		L_GUYPIC_sod
#define LEVELEND_LUMP_END_sod		L_BJWINSPIC_sod

#define TITLESCREEN_LUMP_START_sod	TITLE1PIC_sod
#define TITLESCREEN_LUMP_END_sod	TITLE2PIC_sod

#define ENDGAME1_LUMP_START_sod		ENDSCREEN11PIC_sod
#define ENDGAME1_LUMP_END_sod		ENDSCREEN11PIC_sod

#define ENDGAME2_LUMP_START_sod		ENDSCREEN12PIC_sod
#define ENDGAME2_LUMP_END_sod		ENDSCREEN12PIC_sod

#define EASTEREGG_LUMP_START_sod	IDGUYS1PIC_sod
#define EASTEREGG_LUMP_END_sod		IDGUYS2PIC_sod

#define COPYPROT_LUMP_START_sod		COPYPROTTOPPIC_sod
#define COPYPROT_LUMP_END_sod		BOSSPIC4PIC_sod

#define LATCHPICS_LUMP_START_sod    KNIFEPIC_sod
#define LATCHPICS_LUMP_END_sod		GETPSYCHEDPIC_sod


//
// Amount of each data item
//
#define NUMCHUNKS_sod    ENUMEND_sod
#define NUMFONT_sod      2
#define NUMFONTM_sod     0
#define NUMPICS_sod      (GETPSYCHEDPIC_sod - NUMFONT_sod)
#define NUMPICM_sod      0
#define NUMSPRITES_sod   0
#define NUMTILE8_sod     72
#define NUMTILE8M_sod    0
#define NUMTILE16_sod    0
#define NUMTILE16M_sod   0
#define NUMTILE32_sod    0
#define NUMTILE32M_sod   0
#define NUMEXTERNS_sod   18
//
// File offsets for data items
//
#define STRUCTPIC_sod    0

#define STARTFONT_sod    1
#define STARTFONTM_sod   3
#define STARTPICS_sod    3
#define STARTPICM_sod    TILE8_sod
#define STARTSPRITES_sod TILE8_sod
#define STARTTILE8_sod   TILE8_sod
#define STARTTILE8M_sod  ORDERSCREEN_sod
#define STARTTILE16_sod  ORDERSCREEN_sod
#define STARTTILE16M_sod ORDERSCREEN_sod
#define STARTTILE32_sod  ORDERSCREEN_sod
#define STARTTILE32M_sod ORDERSCREEN_sod
#define STARTEXTERNS_sod ORDERSCREEN_sod

//
// Thank you for using IGRAB!
//
#endif
