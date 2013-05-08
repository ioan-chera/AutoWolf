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
// Graphics .H file for .WL6
// IGRAB-ed on Wed Apr 13 06:58:44 1994
//
//////////////////////////////////////
// IOANCH 20130302: unification
#ifndef GFXV_WL6_H_
#define GFXV_WL6_H_
typedef enum {
    // Lump Start
    H_BJPIC_wl6=3,
    H_CASTLEPIC_wl6,                         // 4
    H_BLAZEPIC_wl6,                          // 5
    H_TOPWINDOWPIC_wl6,                      // 6
    H_LEFTWINDOWPIC_wl6,                     // 7
    H_RIGHTWINDOWPIC_wl6,                    // 8
    H_BOTTOMINFOPIC_wl6,                     // 9
    // Lump Start
    C_OPTIONSPIC_wl6,                        // 10
    C_CURSOR1PIC_wl6,                        // 11
    C_CURSOR2PIC_wl6,                        // 12
    C_NOTSELECTEDPIC_wl6,                    // 13
    C_SELECTEDPIC_wl6,                       // 14
    C_FXTITLEPIC_wl6,                        // 15
    C_DIGITITLEPIC_wl6,                      // 16
    C_MUSICTITLEPIC_wl6,                     // 17
    C_MOUSELBACKPIC_wl6,                     // 18
    C_BABYMODEPIC_wl6,                       // 19
    C_EASYPIC_wl6,                           // 20
    C_NORMALPIC_wl6,                         // 21
    C_HARDPIC_wl6,                           // 22
    C_LOADSAVEDISKPIC_wl6,                   // 23
    C_DISKLOADING1PIC_wl6,                   // 24
    C_DISKLOADING2PIC_wl6,                   // 25
    C_CONTROLPIC_wl6,                        // 26
    C_CUSTOMIZEPIC_wl6,                      // 27
    C_LOADGAMEPIC_wl6,                       // 28
    C_SAVEGAMEPIC_wl6,                       // 29
    C_EPISODE1PIC_wl6,                       // 30
    C_EPISODE2PIC_wl6,                       // 31
    C_EPISODE3PIC_wl6,                       // 32
    C_EPISODE4PIC_wl6,                       // 33
    C_EPISODE5PIC_wl6,                       // 34
    C_EPISODE6PIC_wl6,                       // 35
    C_CODEPIC_wl6,                           // 36
    C_TIMECODEPIC_wl6,                       // 37
    C_LEVELPIC_wl6,                          // 38
    C_NAMEPIC_wl6,                           // 39
    C_SCOREPIC_wl6,                          // 40
    C_JOY1PIC_wl6,                           // 41
    C_JOY2PIC_wl6,                           // 42
    // Lump Start
    L_GUYPIC_wl6,                            // 43
    L_COLONPIC_wl6,                          // 44
    L_NUM0PIC_wl6,                           // 45
    L_NUM1PIC_wl6,                           // 46
    L_NUM2PIC_wl6,                           // 47
    L_NUM3PIC_wl6,                           // 48
    L_NUM4PIC_wl6,                           // 49
    L_NUM5PIC_wl6,                           // 50
    L_NUM6PIC_wl6,                           // 51
    L_NUM7PIC_wl6,                           // 52
    L_NUM8PIC_wl6,                           // 53
    L_NUM9PIC_wl6,                           // 54
    L_PERCENTPIC_wl6,                        // 55
    L_APIC_wl6,                              // 56
    L_BPIC_wl6,                              // 57
    L_CPIC_wl6,                              // 58
    L_DPIC_wl6,                              // 59
    L_EPIC_wl6,                              // 60
    L_FPIC_wl6,                              // 61
    L_GPIC_wl6,                              // 62
    L_HPIC_wl6,                              // 63
    L_IPIC_wl6,                              // 64
    L_JPIC_wl6,                              // 65
    L_KPIC_wl6,                              // 66
    L_LPIC_wl6,                              // 67
    L_MPIC_wl6,                              // 68
    L_NPIC_wl6,                              // 69
    L_OPIC_wl6,                              // 70
    L_PPIC_wl6,                              // 71
    L_QPIC_wl6,                              // 72
    L_RPIC_wl6,                              // 73
    L_SPIC_wl6,                              // 74
    L_TPIC_wl6,                              // 75
    L_UPIC_wl6,                              // 76
    L_VPIC_wl6,                              // 77
    L_WPIC_wl6,                              // 78
    L_XPIC_wl6,                              // 79
    L_YPIC_wl6,                              // 80
    L_ZPIC_wl6,                              // 81
    L_EXPOINTPIC_wl6,                        // 82
    L_APOSTROPHEPIC_wl6,                     // 83
    L_GUY2PIC_wl6,                           // 84
    L_BJWINSPIC_wl6,                         // 85
    STATUSBARPIC_wl6,                        // 86
    TITLEPIC_wl6,                            // 87
    PG13PIC_wl6,                             // 88
    CREDITSPIC_wl6,                          // 89
    HIGHSCORESPIC_wl6,                       // 90
    // Lump Start
    KNIFEPIC_wl6,                            // 91
    GUNPIC_wl6,                              // 92
    MACHINEGUNPIC_wl6,                       // 93
    GATLINGGUNPIC_wl6,                       // 94
    NOKEYPIC_wl6,                            // 95
    GOLDKEYPIC_wl6,                          // 96
    SILVERKEYPIC_wl6,                        // 97
    N_BLANKPIC_wl6,                          // 98
    N_0PIC_wl6,                              // 99
    N_1PIC_wl6,                              // 100
    N_2PIC_wl6,                              // 101
    N_3PIC_wl6,                              // 102
    N_4PIC_wl6,                              // 103
    N_5PIC_wl6,                              // 104
    N_6PIC_wl6,                              // 105
    N_7PIC_wl6,                              // 106
    N_8PIC_wl6,                              // 107
    N_9PIC_wl6,                              // 108
    FACE1APIC_wl6,                           // 109
    FACE1BPIC_wl6,                           // 110
    FACE1CPIC_wl6,                           // 111
    FACE2APIC_wl6,                           // 112
    FACE2BPIC_wl6,                           // 113
    FACE2CPIC_wl6,                           // 114
    FACE3APIC_wl6,                           // 115
    FACE3BPIC_wl6,                           // 116
    FACE3CPIC_wl6,                           // 117
    FACE4APIC_wl6,                           // 118
    FACE4BPIC_wl6,                           // 119
    FACE4CPIC_wl6,                           // 120
    FACE5APIC_wl6,                           // 121
    FACE5BPIC_wl6,                           // 122
    FACE5CPIC_wl6,                           // 123
    FACE6APIC_wl6,                           // 124
    FACE6BPIC_wl6,                           // 125
    FACE6CPIC_wl6,                           // 126
    FACE7APIC_wl6,                           // 127
    FACE7BPIC_wl6,                           // 128
    FACE7CPIC_wl6,                           // 129
    FACE8APIC_wl6,                           // 130
    GOTGATLINGPIC_wl6,                       // 131
    MUTANTBJPIC_wl6,                         // 132
    PAUSEDPIC_wl6,                           // 133
    GETPSYCHEDPIC_wl6,                       // 134



    ORDERSCREEN_wl6=136,
    ERRORSCREEN_wl6,                         // 137
    T_HELPART_wl6,                           // 138
    T_DEMO0_wl6,                             // 139
    T_DEMO1_wl6,                             // 140
    T_DEMO2_wl6,                             // 141
    T_DEMO3_wl6,                             // 142
    T_ENDART1_wl6,                           // 143
    T_ENDART2_wl6,                           // 144
    T_ENDART3_wl6,                           // 145
    T_ENDART4_wl6,                           // 146
    T_ENDART5_wl6,                           // 147
    T_ENDART6_wl6,                           // 148
    ENUMEND_wl6
} graphicnums_wl6;

//
// Data LUMPs
//
#define README_LUMP_START_wl6		3
#define README_LUMP_END_wl6			9

#define CONTROLS_LUMP_START_wl6		10
#define CONTROLS_LUMP_END_wl6		42

#define LEVELEND_LUMP_START_wl6		43
#define LEVELEND_LUMP_END_wl6		85

#define LATCHPICS_LUMP_START_wl6	91
#define LATCHPICS_LUMP_END_wl6		134


//
// Amount of each data item
//
#define NUMCHUNKS_wl6    149
#define NUMFONT_wl6      2
#define NUMFONTM_wl6     0
#define NUMPICS_wl6      132
#define NUMPICM_wl6      0
#define NUMSPRITES_wl6   0
#define NUMTILE8_wl6     72
#define NUMTILE8M_wl6    0
#define NUMTILE16_wl6    0
#define NUMTILE16M_wl6   0
#define NUMTILE32_wl6    0
#define NUMTILE32M_wl6   0
#define NUMEXTERNS_wl6   13
//
// File offsets for data items
//
#define STRUCTPIC_wl6    0

#define STARTFONT_wl6    1
#define STARTFONTM_wl6   3
#define STARTPICS_wl6    3
#define STARTPICM_wl6    135
#define STARTSPRITES_wl6 135
#define STARTTILE8_wl6   135
#define STARTTILE8M_wl6  136
#define STARTTILE16_wl6  136
#define STARTTILE16M_wl6 136
#define STARTTILE32_wl6  136
#define STARTTILE32M_wl6 136
#define STARTEXTERNS_wl6 136

//
// Thank you for using IGRAB!
//
#endif
