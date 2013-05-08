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


#ifndef GFXVABSTRACT_H_
#define GFXVABSTRACT_H_

#include "gfxv_wl6.h"
#include "gfxv_sod.h"

typedef enum
{
    BJCOLLAPSE1PIC,                      // 31
    BJCOLLAPSE2PIC,                      // 32
    BJCOLLAPSE3PIC,                      // 33
    BJCOLLAPSE4PIC,                      // 34
    BJOUCHPIC,                           // 147
    BJWAITING1PIC,                       // 145
    BJWAITING2PIC,                       // 146
    BOSSPIC1PIC,                         // 97
    BOSSPIC2PIC,                         // 98
    BOSSPIC3PIC,                         // 99
    BOSSPIC4PIC,                         // 100
    C_BABYMODEPIC,                       // 19
    C_BACKDROPPIC,
    C_CODEPIC,                           // 36
    C_CONTROLPIC,                        // 15
    C_CURSOR1PIC,                        // 11
    C_CURSOR2PIC,                        // 12
    C_CUSTOMIZEPIC,                      // 27
    C_DIGITITLEPIC,                      // 16
    C_DISKLOADING1PIC,                   // 24
    C_DISKLOADING2PIC,                   // 25
    C_EASYPIC,                           // 20
    C_EPISODE1PIC,                       // 30
    C_EPISODE2PIC,                       // 31
    C_EPISODE3PIC,                       // 32
    C_EPISODE4PIC,                       // 33
    C_EPISODE5PIC,                       // 34
    C_EPISODE6PIC,                       // 35
    C_FXTITLEPIC,                        // 15
    C_HARDPIC,                           // 22
    C_HOWTOUGHPIC,                       // 20
    C_JOY1PIC,                           // 10
    C_JOY2PIC,                           // 11
    C_JOYSTICKPIC,                       // 13
    C_KEYBOARDPIC,                       // 14
    C_LEVELPIC,                          // 38
    C_LOADGAMEPIC,                       // 27
    C_LOADSAVEDISKPIC,                   // 23
    C_MOUSELBACKPIC,                     // 18
    C_MOUSEPIC,                          // 12
    C_MUSICTITLEPIC,                     // 17
    C_NAMEPIC,                           // 39
    C_NORMALPIC,                         // 21
    C_NOTSELECTEDPIC,                    // 13
    C_OPTIONSPIC,                        // 10
    C_SAVEGAMEPIC,                       // 28
    C_SCOREPIC,                          // 40
    C_SELECTEDPIC,                       // 14
    C_TIMECODEPIC,                       // 37
    C_WONSPEARPIC,                       // 30
    COPYPROTBOXPIC,                      // 96
    COPYPROTTOPPIC,                      // 95
    CREDITSPIC,                          // 89
    END1PALETTE,                         // 154
    END2PALETTE,                         // 155
    END3PALETTE,                         // 156
    END4PALETTE,                         // 157
    END5PALETTE,                         // 158
    END6PALETTE,                         // 159
    END7PALETTE,                         // 160
    END8PALETTE,                         // 161
    END9PALETTE,                         // 162
    ENDPICPIC,                           // 35
    ENDSCREEN11PIC,                      // 81
    ENDSCREEN12PIC,                      // 82
    ENDSCREEN3PIC,                       // 83
    ENDSCREEN4PIC,                       // 84
    ENDSCREEN5PIC,                       // 85
    ENDSCREEN6PIC,                       // 86
    ENDSCREEN7PIC,                       // 87
    ENDSCREEN8PIC,                       // 88
    ENDSCREEN9PIC,                       // 89
    ERRORSCREEN,                         // 137
    FACE1APIC,                           // 109
    FACE1BPIC,                           // 110
    FACE1CPIC,                           // 111
    FACE2APIC,                           // 112
    FACE2BPIC,                           // 113
    FACE2CPIC,                           // 114
    FACE3APIC,                           // 115
    FACE3BPIC,                           // 116
    FACE3CPIC,                           // 117
    FACE4APIC,                           // 118
    FACE4BPIC,                           // 119
    FACE4CPIC,                           // 120
    FACE5APIC,                           // 121
    FACE5BPIC,                           // 122
    FACE5CPIC,                           // 123
    FACE6APIC,                           // 124
    FACE6BPIC,                           // 125
    FACE6CPIC,                           // 126
    FACE7APIC,                           // 127
    FACE7BPIC,                           // 128
    FACE7CPIC,                           // 129
    FACE8APIC,                           // 130
    GATLINGGUNPIC,                       // 104
    GETPSYCHEDPIC,                       // 134
    GODMODEFACE1PIC,                     // 142
    GODMODEFACE2PIC,                     // 143
    GODMODEFACE3PIC,                     // 144
    GOLDKEYPIC,                          // 106
    GOTGATLINGPIC,                       // 131
    GUNPIC,                              // 102
    H_BJPIC,
    H_BLAZEPIC,                          // 5
    H_BOTTOMINFOPIC,                     // 9
    H_CASTLEPIC,                         // 4
    H_LEFTWINDOWPIC,                     // 7
    H_RIGHTWINDOWPIC,                    // 8
    H_TOPWINDOWPIC,                      // 6
    HIGHSCORESPIC,                       // 29
    IDGUYS1PIC,                          // 93
    IDGUYS2PIC,                          // 94
    IDGUYSPALETTE,                       // 163
    KNIFEPIC,                            // 101
    L_APIC,                              // 49
    L_APOSTROPHEPIC,                     // 76
    L_BJWINSPIC,                         // 78
    L_BPIC,                              // 50
    L_COLONPIC,                          // 37
    L_CPIC,                              // 51
    L_DPIC,                              // 52
    L_EPIC,                              // 53
    L_EXPOINTPIC,                        // 75
    L_FPIC,                              // 54
    L_GPIC,                              // 55
    L_GUY2PIC,                           // 77
    L_GUYPIC,                            // 36
    L_HPIC,                              // 56
    L_IPIC,                              // 57
    L_JPIC,                              // 58
    L_KPIC,                              // 59
    L_LPIC,                              // 60
    L_MPIC,                              // 61
    L_NPIC,                              // 62
    L_NUM0PIC,                           // 38
    L_NUM1PIC,                           // 39
    L_NUM2PIC,                           // 47
    L_NUM3PIC,                           // 48
    L_NUM4PIC,                           // 49
    L_NUM5PIC,                           // 50
    L_NUM6PIC,                           // 51
    L_NUM7PIC,                           // 52
    L_NUM8PIC,                           // 53
    L_NUM9PIC,                           // 54
    L_OPIC,                              // 70
    L_PERCENTPIC,                        // 55
    L_PPIC,                              // 71
    L_QPIC,                              // 72
    L_RPIC,                              // 73
    L_SPIC,                              // 74
    L_TPIC,                              // 75
    L_UPIC,                              // 76
    L_VPIC,                              // 77
    L_WPIC,                              // 78
    L_XPIC,                              // 79
    L_YPIC,                              // 80
    L_ZPIC,                              // 81
    MACHINEGUNPIC,                       // 93
    MUTANTBJPIC,                         // 132
    N_0PIC,                              // 99
    N_1PIC,                              // 110
    N_2PIC,                              // 111
    N_3PIC,                              // 112
    N_4PIC,                              // 113
    N_5PIC,                              // 114
    N_6PIC,                              // 115
    N_7PIC,                              // 116
    N_8PIC,                              // 117
    N_9PIC,                              // 118
    N_BLANKPIC,                          // 98
    NOKEYPIC,                            // 95
    ORDERSCREEN,
    PAUSEDPIC,                           // 148
    PG13PIC,                             // 91
    SILVERKEYPIC,                        // 97
    STATUSBARPIC,                        // 90
    T_DEMO0,                             // 164
    T_DEMO1,                             // 165
    T_DEMO2,                             // 166
    T_DEMO3,                             // 167
    T_ENDART1,                           // 168
    T_ENDART2,                           // 144
    T_ENDART3,                           // 145
    T_ENDART4,                           // 146
    T_ENDART5,                           // 147
    T_ENDART6,                           // 148
    T_HELPART,                           // 138
    TILE8,                               // 150
    TITLE1PIC,                           // 79
    TITLE2PIC,                           // 80
    TITLEPALETTE,                        // 153
    TITLEPIC,                            // 87

    // DEFINE maps
    BACKDROP_LUMP_END,
    BACKDROP_LUMP_START,
    CONTROL_LUMP_END,
    CONTROL_LUMP_START,
    CONTROLS_LUMP_END,
    CONTROLS_LUMP_START,
    COPYPROT_LUMP_END,
    COPYPROT_LUMP_START,
    EASTEREGG_LUMP_END,
    EASTEREGG_LUMP_START,
    ENDGAME1_LUMP_END,
    ENDGAME1_LUMP_START,
    ENDGAME2_LUMP_END,
    ENDGAME2_LUMP_START,
    ENDGAME_LUMP_END,
    ENDGAME_LUMP_START,
    HIGHSCORES_LUMP_END,
    HIGHSCORES_LUMP_START,
    LATCHPICS_LUMP_END,
    LATCHPICS_LUMP_START,
    LEVELEND_LUMP_END,
    LEVELEND_LUMP_START,
    LOADSAVE_LUMP_END,
    LOADSAVE_LUMP_START,
    NEWGAME_LUMP_END,
    NEWGAME_LUMP_START,
    NUMCHUNKS,
    NUMEXTERNS,
    NUMFONT,
    NUMFONTM,
    NUMPICM,
    NUMPICS,
    NUMSPRITES,
    NUMTILE16,
    NUMTILE16M,
    NUMTILE32,
    NUMTILE32M,
    NUMTILE8,
    NUMTILE8M,
    OPTIONS_LUMP_END,
    OPTIONS_LUMP_START,
    README_LUMP_END,
    README_LUMP_START,
    SOUND_LUMP_END,
    SOUND_LUMP_START,
    STARTEXTERNS,
    STARTFONT,
    STARTFONTM,
    STARTPICM,
    STARTPICS,
    STARTSPRITES,
    STARTTILE16,
    STARTTILE16M,
    STARTTILE32,
    STARTTILE32M,
    STARTTILE8,
    STARTTILE8M,
    STRUCTPIC,
    TITLESCREEN_LUMP_END,
    TITLESCREEN_LUMP_START,

    ENUMEND_abstract,
} graphicnums_abstract;

extern const unsigned int gfxvmap[][2];

#endif
