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

//
//      ID Engine
//      ID_SD.h - Sound Manager Header
//      Version for Wolfenstein
//      By Jason Blochowiak
//

#ifndef __ID_SD__
#define __ID_SD__


#define alOut(n,b) SD_L_YM3812Write(sd_oplChip, n, b)

#define TickBase        70      // 70Hz per tick - used as a base for timer 0

typedef enum
{
    sdm_Off,
    sdm_PC,sdm_AdLib,
} SDMode;

typedef enum
{
    smm_Off,smm_AdLib
} SMMode;

typedef enum
{
    sds_Off,sds_PC,sds_SoundBlaster
} SDSMode;

typedef struct
{
    longword        length;
    word            priority;
} SoundCommon;

#define ORIG_SOUNDCOMMON_SIZE 6

//      PC Sound stuff
#define pcTimer         0x42
#define pcTAccess       0x43
#define pcSpeaker       0x61

#define pcSpkBits       3

typedef struct
{
    SoundCommon     common;
    byte            data[1];
} PCSound;

//      Register addresses
// Operator stuff
#define alChar          0x20
#define alScale         0x40
#define alAttack        0x60
#define alSus           0x80
#define alWave          0xe0
// Channel stuff
#define alFreqL         0xa0
#define alFreqH         0xb0
#define alFeedCon       0xc0
// Global stuff
#define alEffects       0xbd

typedef struct
{
    byte    mChar,cChar,
            mScale,cScale,
            mAttack,cAttack,
            mSus,cSus,
            mWave,cWave,
            nConn,

            // These are only for Muse - these bytes are really unused
            voice,
            mode;
    byte    unused[3];
} Instrument;

#define ORIG_INSTRUMENT_SIZE 16

typedef struct
{
    SoundCommon     common;
    Instrument      inst;
    byte            block;
    byte            data[1];
} AdLibSound;

#define ORIG_ADLIBSOUND_SIZE (ORIG_SOUNDCOMMON_SIZE + ORIG_INSTRUMENT_SIZE + 2)

//
//      Sequencing stuff
//
#define sqMaxTracks     10

typedef struct
{
    word    length;
    word    values[1];
} MusicGroup;

typedef struct
{
    int valid;
    fixed globalsoundx, globalsoundy;
} globalsoundpos;

extern globalsoundpos channelSoundPos[];

// Global variables
extern  Boolean         sd_adLibPresent,
                        sd_soundBlasterPresent,
                        sd_soundPositioned;
extern  SDMode          SoundMode;
extern  SDSMode         sd_digiMode;
extern  SMMode          sd_musicMode;
extern  int             sd_digiMap[];
extern  int             sd_digiChannel[];

uint32_t I_GetTicks(void);
#define GetTimeCount()  ((I_GetTicks()*7)/100)

void I_Delay(unsigned ms);
inline void Delay(int wolfticks)
{
    if(wolfticks>0) I_Delay(wolfticks * 100 / 7);
}

// Function prototypes
extern  void    SD_Startup(),
                SD_Shutdown();

extern  int     SD_GetChannelForDigi(int which);
extern  void    SD_PositionSound(int leftvol,int rightvol);
extern  Boolean SD_PlaySound(soundnames sound);
extern  void    SD_SetPosition(int channel, int leftvol,int rightvol);
extern  void    SD_StopSound(),
                SD_WaitSoundDone();

extern  void    SD_StartMusic(int chunk);
extern  void    SD_ContinueMusic(int chunk, int startoffs);
extern  void    SD_MusicOn(),
                SD_FadeOutMusic();
extern  int     SD_MusicOff();

extern  Boolean SD_MusicPlaying();
extern  Boolean SD_SetSoundMode(SDMode mode);
extern  Boolean SD_SetMusicMode(SMMode mode);
extern  word    SD_SoundPlaying();

extern  void    SD_SetDigiDevice(SDSMode);
extern  void	SD_PrepareSound(int which);
extern  int     SD_PlayDigitized(word which,int leftpos,int rightpos);
extern  void    SD_StopDigitized();

#endif
