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
//      ID_SD.c - Sound Manager for Wolfenstein 3D
//      v1.2
//      By Jason Blochowiak
//

//
//      This module handles dealing with generating sound on the appropriate
//              hardware
//
//      Depends on: User Mgr (for parm checking)
//
//      Globals:
//              For User Mgr:
//                      sd_soundBlasterPresent - SoundBlaster card present?
//                      AdLibPresent - AdLib card present?
//                      sd_soundMode - What device is used for sound effects
//                              (Use SM_SetSoundMode() to set)
//                      sd_musicMode - What device is used for music
//                              (Use SM_SetMusicMode() to set)
//                      sd_digiMode - What device is used for digitized sound effects
//                              (Use SM_SetDigiDevice() to set)
//
//              For Cache Mgr:
//                      NeedsDigitized - load digitized sounds?
//                      NeedsMusic - load music?
//

// IOANCH 20121223: Cleaned up this file

#include "version.h"
#include "wl_def.h"
#include "i_system.h"
#include "id_ca.h"
#include "id_pm.h"
#include "id_sd.h"
#include "Logger.h"
#include "wl_main.h"

#if defined(GP2X_940)
#include "gp2x/fmopl.h"
#else
#ifdef USE_GPL
#include "dosbox/dbopl.h"
#else
#include "mame/fmopl.h"
#endif
#endif
#define ORIGSAMPLERATE 7042

#include "Config.h"

 struct headchunk
{
	char RIFF[4];
	longword filelenminus8;
	char WAVE[4];
	char fmt_[4];
	longword formatlen;
	word val0x0001;
	word channels;
	longword samplerate;
	longword bytespersec;
	word bytespersample;
	word bitspersample;
} ;

struct wavechunk
{
	char chunkid[4];
	longword chunklength;
};

struct  digiinfo
{
    uint32_t startpage;
    uint32_t length;
};
// IOANCH 20130301: unification
static Mix_Chunk *sd_soundChunks[STARTMUSIC_wl6 - STARTDIGISOUNDS_wl6 > 
							  STARTMUSIC_sod - STARTDIGISOUNDS_sod ?
							  STARTMUSIC_wl6 - STARTDIGISOUNDS_wl6 :
							  STARTMUSIC_sod - STARTDIGISOUNDS_sod];
static byte      *SoundBuffers[STARTMUSIC_wl6 - STARTDIGISOUNDS_wl6 > 
							   STARTMUSIC_sod - STARTDIGISOUNDS_sod ?
							   STARTMUSIC_wl6 - STARTDIGISOUNDS_wl6 :
							   STARTMUSIC_sod - STARTDIGISOUNDS_sod];

struct globalsoundpos;
globalsoundpos channelSoundPos[MIX_CHANNELS];

//      Global variables
        Boolean8         sd_adLibPresent,
                        sd_soundBlasterPresent,SBProPresent,
                        sd_soundPositioned;
        SDMode          sd_soundMode;
        SMMode          sd_musicMode;
        SDSMode         sd_digiMode;
static  const byte          * const*SoundTable;

// IOANCH 20130301: unification
int             sd_digiMap[(unsigned int)LASTSOUND_wl6 > 
						(unsigned int)LASTSOUND_sod ? 
						(unsigned int)LASTSOUND_wl6 : 
						(unsigned int)LASTSOUND_sod];
int sd_digiChannel[STARTMUSIC_wl6 - STARTDIGISOUNDS_wl6 > STARTMUSIC_sod - 
				STARTDIGISOUNDS_sod ? STARTMUSIC_wl6 - STARTDIGISOUNDS_wl6 :
				STARTMUSIC_sod - STARTDIGISOUNDS_sod];

//      Internal variables
static  Boolean8                 SD_Started;
static  Boolean8                 sd_nextSoundPos;
// IOANCH 20130301: unification
static  unsigned int            sd_soundNumber;
static  unsigned int            sd_digiNumber;
static  word                    sd_soundPriority;
static  word                    sd_digiPriority;
static  int                     sd_leftPosition;
static  int                     sd_rightPosition;

        word                    sd_numDigi;
static  digiinfo               *sd_digiList;
static  Boolean8                 sd_digiPlaying;

//      PC Sound variables
// static  volatile byte           pcLastSample;
static  byte * volatile         pcSound;
// IOANCH 11.06.2012: commented to made compiler stop complaining
//static  longword                pcLengthLeft;

//      AdLib variables
static  byte * volatile         sd_alSound;
static  byte                    sd_alBlock;
static  longword                sd_alLengthLeft;
static  longword                sd_alTimeCount;
static  Instrument              alZeroInst;

//      Sequencer variables
static  volatile Boolean8        sd_sqActive;
static  const word                   *sd_sqHack;
static  const word                   *sd_sqHackPtr;
static  int                     sd_sqHackLen;
static  int                     sd_sqHackSeqLen;
static  longword                sd_sqHackTime;

#ifdef USE_GPL

DBOPL::Chip sd_oplChip;

//
// SD_L_YM3812Init
//
static inline bool SD_L_YM3812Init(int numChips, int clock, int rate)
{
	sd_oplChip.Setup(rate);
	return false;
}

//
// SD_L_YM3812Write
//
static inline void SD_L_YM3812Write(DBOPL::Chip &which, Bit32u reg, Bit8u val)
{
	which.WriteReg(reg, val);
}

//
// SD_L_YM3812UpdateOne
//
static inline void SD_L_YM3812UpdateOne(DBOPL::Chip &which, int16_t *stream, int length)
{
	Bit32s buffer[512 * 2];
	int i;

	// length is at maximum sd_samplesPerMusicTick = param_samplerate / 700
	// so 512 is sufficient for a sample rate of 358.4 kHz (default 44.1 kHz)
	if(length > 512)
		length = 512;

	if(which.opl3Active)
	{
		which.GenerateBlock3(length, buffer);

		// GenerateBlock3 generates a number of "length" 32-bit stereo samples
		// so we only need to convert them to 16-bit samples
		for(i = 0; i < length * 2; i++)  // * 2 for left/right channel
		{
			// Multiply by 4 to match loudness of MAME emulator.
			Bit32s sample = buffer[i] << 2;
			if(sample > 32767) sample = 32767;
			else if(sample < -32768) sample = -32768;
			stream[i] = sample;
		}
	}
	else
	{
		which.GenerateBlock2(length, buffer);

		// GenerateBlock3 generates a number of "length" 32-bit mono samples
		// so we need to convert them to 32-bit stereo samples
		for(i = 0; i < length; i++)
		{
			// Multiply by 4 to match loudness of MAME emulator.
			// Then upconvert to stereo.
			Bit32s sample = buffer[i] << 2;
			if(sample > 32767) sample = 32767;
			else if(sample < -32768) sample = -32768;
			stream[i * 2] = stream[i * 2 + 1] = (int16_t) sample;
		}
	}
}

#else

static const int sd_oplChip = 0;

#endif

//
// SD_L_SoundFinished
//
static void SD_L_SoundFinished()
{
    // IOANCH 20130301: unification
	sd_soundNumber   = 0;
	sd_soundPriority = 0;
}

//
// SD_StopDigitized
//
void Sound::StopDigitized()
{
    sd_digiPlaying = false;
    // IOANCH 20130301: unification
    sd_digiNumber = 0;
    sd_digiPriority = 0;
    sd_soundPositioned = false;
    if ((sd_digiMode == sds_PC) && (sd_soundMode == sdm_PC))
        SD_L_SoundFinished();

    switch (sd_digiMode)
    {
        case sds_PC:
//            SD_L_PCStopSampleInIRQ();
            break;
        case sds_SoundBlaster:
//            SD_L_SBStopSampleInIRQ();
            Mix_HaltChannel(-1);
            break;
		default:
			;
    }
}

//
// SD_GetChannelForDigi
//
int SD_GetChannelForDigi(int which)
{
    if(sd_digiChannel[which] != -1) return sd_digiChannel[which];

    int channel = Mix_GroupAvailable(1);
    if(channel == -1) channel = Mix_GroupOldest(1);
    if(channel == -1)           // All sounds stopped in the meantime?
        return Mix_GroupAvailable(1);
    return channel;
}

//
// SD_SetPosition
//
void SD_SetPosition(int channel, int leftpos, int rightpos)
{
    if((leftpos < 0) || (leftpos > 15) || (rightpos < 0) || (rightpos > 15)
            || ((leftpos == 15) && (rightpos == 15)))
        throw Exception("SD_SetPosition: Illegal position");

    switch (sd_digiMode)
    {
        case sds_SoundBlaster:
//            SD_L_PositionSBP(leftpos,rightpos);
            Mix_SetPanning(channel, ((15 - leftpos) << 4) + 15,
                ((15 - rightpos) << 4) + 15);
            break;
		default:
			;
    }
}

//
// GetSample
//
Sint16 GetSample(float csample, const byte *samples, int size)
{
    float s0 = 0, s1 = 0, s2 = 0;
    int cursample = (int)csample;
    float sf = csample - (float)cursample;

    if(cursample - 1 >= 0)
		s0 = (float) (samples[cursample - 1] - 128);
    s1 = (float)(samples[cursample] - 128);
    if(cursample + 1 < size)
		s2 = (float) (samples[cursample + 1] - 128);

    float val = s0 * sf * (sf - 1) / 2 - s1 * (sf * sf - 1) + s2 * (sf + 1) * sf / 2;
    int32_t intval = (int32_t) (val * 256);
    if(intval < -32768)
		intval = -32768;
    else if(intval > 32767)
		intval = 32767;
    return (Sint16) intval;
}

//
// SD_PrepareSound
//
void SD_PrepareSound(int which)
{
    if(sd_digiList == NULL)
        throw Exception((PString("SD_PrepareSound(")<<which<<"): sd_digiList not initialized!")());

    int page = sd_digiList[which].startpage;
    int size = sd_digiList[which].length;

    const byte *origsamples = (const byte *)vSwapData.getSound(page);
    if(origsamples + size >= vSwapData.getEnd())
        throw Exception((PString("SD_PrepareSound(")<<which<<"): Sound reaches out of page file!\n")());

    int destsamples = (int) ((float) size * (float) cfg_samplerate
        / (float) ORIGSAMPLERATE);

   // IOANCH: check
    byte *wavebuffer = (byte *) I_CheckedMalloc(sizeof(headchunk) +
                                                sizeof(wavechunk) +
                                                destsamples * 2);
   // dest are 16-bit samples

    headchunk head = {{'R','I','F','F'}, 0, {'W','A','V','E'},
        {'f','m','t',' '}, 0x10, 0x0001, 1, static_cast<longword>(cfg_samplerate),
        static_cast<longword>(cfg_samplerate*2), 2, 16};
    wavechunk dhead = {{'d', 'a', 't', 'a'}, static_cast<longword>(destsamples*2)};
    head.filelenminus8 = sizeof(head) + destsamples*2;  // (sizeof(dhead)-8 = 0)
    memcpy(wavebuffer, &head, sizeof(head));
    memcpy(wavebuffer+sizeof(head), &dhead, sizeof(dhead));

    // alignment is correct, as wavebuffer comes from malloc
    // and sizeof(headchunk) % 4 == 0 and sizeof(wavechunk) % 4 == 0
    Sint16 *newsamples = (Sint16 *)(void *) (wavebuffer + sizeof(headchunk)
        + sizeof(wavechunk));
    float cursample = 0.F;
    float samplestep = (float) ORIGSAMPLERATE / (float) cfg_samplerate;
    for(int i=0; i<destsamples; i++, cursample+=samplestep)
    {
        newsamples[i] = GetSample((float)size * (float)i / (float)destsamples,
            origsamples, size);
    }
    SoundBuffers[which] = wavebuffer;

    sd_soundChunks[which] = Mix_LoadWAV_RW(SDL_RWFromMem(wavebuffer,
        sizeof(headchunk) + sizeof(wavechunk) + destsamples * 2), 1);
}

//
// SD_PlayDigitized
//
int SD_PlayDigitized(word which,int leftpos,int rightpos)
{
    if (!sd_digiMode)
        return 0;

    if (which >= sd_numDigi)
        throw Exception((PString("SD_PlayDigitized: bad sound number ")<<which)());

    int channel = SD_GetChannelForDigi(which);
    SD_SetPosition(channel, leftpos,rightpos);

    sd_digiPlaying = true;

    Mix_Chunk *sample = sd_soundChunks[which];
    if(sample == NULL)
    {
        Logger::Write("sd_soundChunks[%i] is NULL!", which);
        return 0;
    }

    if(Mix_PlayChannel(channel, sample, 0) == -1)
    {
        Logger::Write("Unable to play sound: %s", Mix_GetError());
        return 0;
    }

    return channel;
}

//
// SD_ChannelFinished
//
void SD_ChannelFinished(int channel)
{
    channelSoundPos[channel].valid = 0;
}

//
// SD_SetDigiDevice
//
void SD_SetDigiDevice(SDSMode mode)
{
    Boolean8 devicenotpresent;

    if (mode == sd_digiMode)
        return;

    Sound::StopDigitized();

    devicenotpresent = false;
    switch (mode)
    {
        case sds_SoundBlaster:
            if (!sd_soundBlasterPresent)
                devicenotpresent = true;
            break;
		default:
			;
    }

    if (!devicenotpresent)
    {
        sd_digiMode = mode;

#ifdef NOTYET
        SD_L_SetTimerSpeed();
#endif
    }
}

//
// SD_L_SetupDigi
//
void SD_L_SetupDigi()
{
    // Correct padding enforced by PM_Startup()
    const word *soundInfoPage = (const word *) (const void *)
         vSwapData[vSwapData.numChunks() - 1];
    sd_numDigi = (word) vSwapData.getPageSize(vSwapData.numChunks() - 1) / 4;

    sd_digiList = (digiinfo *) malloc(sd_numDigi * sizeof(digiinfo));
    int i;
    for(i = 0; i < sd_numDigi; i++)
    {
        // Calculate the size of the digi from the sizes of the pages between
        // the start page and the start page of the next sound

        sd_digiList[i].startpage = soundInfoPage[i * 2];
        if((int) sd_digiList[i].startpage >= vSwapData.numChunks() - 1)
        {
            sd_numDigi = i;
            break;
        }

        int lastPage;
        if(i < sd_numDigi - 1)
        {
            lastPage = soundInfoPage[i * 2 + 2];
            if(lastPage == 0 || lastPage + vSwapData.soundStart() >
               vSwapData.numChunks() - 1)
               lastPage = vSwapData.numChunks() - 1;
            else lastPage += vSwapData.soundStart();
        }
        else lastPage = vSwapData.numChunks() - 1;

        int size = 0;
        for(int page = vSwapData.soundStart() + sd_digiList[i].startpage;
            page < lastPage; page++)
            size += vSwapData.getPageSize(page);

        // Don't include padding of sound info page, if padding was added
        if(lastPage == vSwapData.numChunks() - 1 &&
           vSwapData.soundInfoPagePadded()) size--;

        // Patch lower 16-bit of size with size from sound info page.
        // The original VSWAP contains padding which is included in the page size,
        // but not included in the 16-bit size. So we use the more precise value.
        if((size & 0xffff0000) != 0 && (size & 0xffff) < soundInfoPage[i * 2 + 1])
            size -= 0x10000;
        size = (size & 0xffff0000) | soundInfoPage[i * 2 + 1];

        sd_digiList[i].length = size;
    }

	// IOANCH 20130301: unification
	unsigned int LASTSOUND_max = IMPALE((unsigned int)LASTSOUND);
    for(i = 0; i < (signed int)LASTSOUND_max; i++)
    {
        sd_digiMap[i] = -1;
        sd_digiChannel[i] = -1;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
//      AdLib Code
//
////////////////////////////////////////////////////////////////////////////////

//
// SD_L_ALStopSound
//
// Turns off any sound effects playing through the AdLib card
//              
static void SD_L_ALStopSound()
{
    sd_alSound = 0;
    alOut(alFreqH + 0, 0);
}

//
// SD_L_AlSetFXInst
//
static void SD_L_AlSetFXInst(Instrument *inst)
{
    byte c,m;

    m = 0;      // modulator cell for channel 0
    c = 3;      // carrier cell for channel 0
    alOut(m + alChar,inst->mChar);
    alOut(m + alScale,inst->mScale);
    alOut(m + alAttack,inst->mAttack);
    alOut(m + alSus,inst->mSus);
    alOut(m + alWave,inst->mWave);
    alOut(c + alChar,inst->cChar);
    alOut(c + alScale,inst->cScale);
    alOut(c + alAttack,inst->cAttack);
    alOut(c + alSus,inst->cSus);
    alOut(c + alWave,inst->cWave);

    // Note: Switch commenting on these lines for old MUSE compatibility
//    alOutInIRQ(alFeedCon,inst->nConn);
    alOut(alFeedCon,0);
}

//
// SD_L_ALPlaySound
//
// Plays the specified sound on the AdLib card
//
static void SD_L_ALPlaySound(AdLibSound *sound)
{
    Instrument      *inst;
    byte            *data;

    SD_L_ALStopSound();

    sd_alLengthLeft = sound->common.length;
    data = sound->data;
    sd_alBlock = ((sound->block & 7) << 2) | 0x20;
    inst = &sound->inst;

    if (!(inst->mSus | inst->cSus))
    {
        throw Exception("SD_L_ALPlaySound() - Bad instrument");
    }

    SD_L_AlSetFXInst(inst);
    sd_alSound = (byte *)data;
}

//
// SD_L_ShutAL
//
// Shuts down the AdLib card for sound effects
//
static void SD_L_ShutAL()
{
    sd_alSound = 0;
    alOut(alEffects,0);
    alOut(alFreqH + 0,0);
    SD_L_AlSetFXInst(&alZeroInst);
}

//
// SD_L_CleanAL
//
// Totally shuts down the AdLib card
//

// deleted

//
// SD_L_StartAL
//
// Starts up the AdLib card for sound effects
//
static void SD_L_StartAL()
{
    alOut(alEffects, 0);
    SD_L_AlSetFXInst(&alZeroInst);
}

// Deleted SD_L_DetectAdLib

//
// SD_L_ShutDevice 
//
// turns off whatever device was being used for sound fx
//
static void SD_L_ShutDevice()
{
    switch (sd_soundMode)
    {
        case sdm_PC:
//            SD_L_ShutPC();
            break;
        case sdm_AdLib:
            SD_L_ShutAL();
            break;
		default:
			;
    }
    sd_soundMode = sdm_Off;
}

////////////////////////////////////////////////////////////////////////////////
//
//      SD_L_CleanDevice() - totally shuts down all sound devices
//
////////////////////////////////////////////////////////////////////////////////

// Deleted

////////////////////////////////////////////////////////////////////////////////
//
//      SD_L_StartDevice() - turns on whatever device is to be used for 
// sound fx
//
////////////////////////////////////////////////////////////////////////////////
static void SD_L_StartDevice()
{
    switch (sd_soundMode)
    {
        case sdm_AdLib:
            SD_L_StartAL();
            break;
		default:
			;
    }
    // IOANCH 20130301: unification
    sd_soundNumber = 0;
    sd_soundPriority = 0;
}

//      Public routines

////////////////////////////////////////////////////////////////////////////////
//
//      SD_SetSoundMode() - Sets which sound hardware to use for sound 
// effects
//
////////////////////////////////////////////////////////////////////////////////
Boolean8 SD_SetSoundMode(SDMode mode)
{
    Boolean8 result = false;
    word    tableoffset;

	Sound::Stop();

    if ((mode == sdm_AdLib) && !sd_adLibPresent)
        mode = sdm_PC;

    switch (mode)
    {
        case sdm_Off:
			// IOANCH 20130301: unification
            tableoffset = IMPALE((word)STARTADLIBSOUNDS);
            result = true;
            break;
        case sdm_PC:
            tableoffset = IMPALE((word)STARTPCSOUNDS);
            result = true;
            break;
        case sdm_AdLib:
            tableoffset = IMPALE((word)STARTADLIBSOUNDS);
            if (sd_adLibPresent)
                result = true;
            break;
        default:
            throw Exception(PString("SD_SetSoundMode: Invalid sound mode ").concat(mode)());
            return false;
    }
    SoundTable = audioSegs.ptr(tableoffset);

    if (result && (mode != sd_soundMode))
    {
        SD_L_ShutDevice();
        sd_soundMode = mode;
        SD_L_StartDevice();
    }

    return(result);
}

////////////////////////////////////////////////////////////////////////////////
//
//      SD_SetMusicMode() - sets the device to use for background music
//
////////////////////////////////////////////////////////////////////////////////
Boolean8 SD_SetMusicMode(SMMode mode)
{
    Boolean8 result = false;

    SD_FadeOutMusic();
    while (SD_MusicPlaying())
        I_Delay(5);

    switch (mode)
    {
        case smm_Off:
            result = true;
            break;
        case smm_AdLib:
            if (sd_adLibPresent)
                result = true;
            break;
    }

    if (result)
        sd_musicMode = mode;

//    SD_L_SetTimerSpeed();

    return(result);
}

int sd_numReadySamples = 0;
byte *sd_curAlSound = 0;
byte *sd_curAlSoundPtr = 0;
longword sd_curAlLengthLeft = 0;
int sd_soundTimeCounter = 5;
int sd_samplesPerMusicTick;

//
// SD_L_IMFMusicPlayer
//
void SD_L_IMFMusicPlayer(void *udata, Uint8 *stream, int len)
{
    int stereolen = len>>1;
    int sampleslen = stereolen>>1;
    int16_t *stream16 = (int16_t *) (void *) stream;    // expect correct alignment

    while(1)
    {
        if(sd_numReadySamples)
        {
            if(sd_numReadySamples<sampleslen)
            {
                SD_L_YM3812UpdateOne(sd_oplChip, stream16, sd_numReadySamples);
                stream16 += sd_numReadySamples*2;
                sampleslen -= sd_numReadySamples;
            }
            else
            {
                SD_L_YM3812UpdateOne(sd_oplChip, stream16, sampleslen);
                sd_numReadySamples -= sampleslen;
                return;
            }
        }
        sd_soundTimeCounter--;
        if(!sd_soundTimeCounter)
        {
            sd_soundTimeCounter = 5;
            if(sd_curAlSound != sd_alSound)
            {
                sd_curAlSound = sd_curAlSoundPtr = sd_alSound;
                sd_curAlLengthLeft = sd_alLengthLeft;
            }
            if(sd_curAlSound)
            {
                if(*sd_curAlSoundPtr)
                {
                    alOut(alFreqL, *sd_curAlSoundPtr);
                    alOut(alFreqH, sd_alBlock);
                }
                else alOut(alFreqH, 0);
                sd_curAlSoundPtr++;
                sd_curAlLengthLeft--;
                if(!sd_curAlLengthLeft)
                {
                    sd_curAlSound = sd_alSound = 0;
                    // IOANCH 20130301: unification
                    sd_soundNumber = 0;
                    sd_soundPriority = 0;
                    alOut(alFreqH, 0);
                }
            }
        }
        if(sd_sqActive && sd_sqHackPtr)
        {
            do
            {
                if(sd_sqHackTime > sd_alTimeCount) break;
                sd_sqHackTime = sd_alTimeCount + *(sd_sqHackPtr+1);
                alOut(*(byte *) sd_sqHackPtr, *(((byte *) sd_sqHackPtr)+1));
                sd_sqHackPtr += 2;
                sd_sqHackLen -= 4;
            }
            while(sd_sqHackLen>0);
            sd_alTimeCount++;
            if(!sd_sqHackLen)
            {
                sd_sqHackPtr = sd_sqHack;
                sd_sqHackLen = sd_sqHackSeqLen;
                sd_sqHackTime = 0;
                sd_alTimeCount = 0;
            }
        }
        sd_numReadySamples = sd_samplesPerMusicTick;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
//      SD_Startup() - starts up the Sound Mgr
//              Detects all additional sound hardware and installs my ISR
//
////////////////////////////////////////////////////////////////////////////////
void SD_Startup()
{
    int     i;

    if (SD_Started)
        return;

    if(Mix_OpenAudio(cfg_samplerate, AUDIO_S16, 2, cfg_audiobuffer))
    {
        Logger::Write("Unable to open audio: %s", Mix_GetError());
        return;
    }

    Mix_ReserveChannels(2);  // reserve player and boss weapon channels
    Mix_GroupChannels(2, MIX_CHANNELS-1, 1); // group remaining channels

    // Init music

    sd_samplesPerMusicTick = cfg_samplerate / 700;
   // SD_L_t0FastAsmService played at 700Hz

    if(SD_L_YM3812Init(1,3579545,cfg_samplerate))
    {
        Logger::Write("Unable to create virtual OPL!!");
    }

    for(i=1;i<0xf6;i++)
        SD_L_YM3812Write(sd_oplChip,i,0);

    SD_L_YM3812Write(sd_oplChip,1,0x20); // Set WSE=1
//    SD_L_YM3812Write(0,8,0); // Set CSM=0 & SEL=0
   // already set in for statement

    Mix_HookMusic(SD_L_IMFMusicPlayer, 0);
    Mix_ChannelFinished(SD_ChannelFinished);
    sd_adLibPresent = true;
    sd_soundBlasterPresent = true;

    sd_alTimeCount = 0;

    SD_SetSoundMode(sdm_Off);
    SD_SetMusicMode(smm_Off);

    SD_L_SetupDigi();

    SD_Started = true;
}

////////////////////////////////////////////////////////////////////////////////
//
//      SD_Shutdown() - shuts down the Sound Mgr
//              Removes sound ISR and turns off whatever sound hardware was 
// active
//
////////////////////////////////////////////////////////////////////////////////
void SD_Shutdown()
{
    if (!SD_Started)
        return;

	Sound::MusicOff();
	Sound::Stop();

	unsigned int lastvalue = SPEAR::flag ? STARTMUSIC_sod - STARTDIGISOUNDS_sod :
    STARTMUSIC_wl6 - STARTDIGISOUNDS_wl6;
    
    for(int i = 0; i < (signed int)lastvalue; i++)
    {
        if(sd_soundChunks[i]) Mix_FreeChunk(sd_soundChunks[i]);
        if(SoundBuffers[i]) free(SoundBuffers[i]);
    }

    free(sd_digiList);

    SD_Started = false;
}

////////////////////////////////////////////////////////////////////////////////
//
//      SD_PositionSound() - Sets up a stereo imaging location for the next
//              sound to be played. Each channel ranges from 0 to 15.
//
////////////////////////////////////////////////////////////////////////////////
void SD_PositionSound(int leftvol,int rightvol)
{
    sd_leftPosition = leftvol;
    sd_rightPosition = rightvol;
    sd_nextSoundPos = true;
}

////////////////////////////////////////////////////////////////////////////////
//
//      Sound::Play() - plays the specified sound on the appropriate 
// hardware
//
////////////////////////////////////////////////////////////////////////////////
Boolean8 Sound::Play(soundnames sound_abstract)
{
    Boolean8         ispos;
    SoundCommon     *s;
    int             lp,rp;
	
	// IOANCH 20130301: abstract sound
	unsigned int sound = SPEAR::sd(sound_abstract);

    lp = sd_leftPosition;
    rp = sd_rightPosition;
    sd_leftPosition = 0;
    sd_rightPosition = 0;

    ispos = sd_nextSoundPos;
    sd_nextSoundPos = false;

    if (sd_digiMode == sds_Off && sd_soundMode == sdm_Off)
        return 0;

    s = (SoundCommon *) SoundTable[sound];

    if ((sd_soundMode != sdm_Off) && !s)
            throw Exception("Sound::Play() - Uncached sound");

    if ((sd_digiMode != sds_Off) && (sd_digiMap[sound] != -1))
    {
        if ((sd_digiMode == sds_PC) && (sd_soundMode == sdm_PC))
        {
#ifdef NOTYET
            if (s->priority < sd_soundPriority)
                return 0;

            SD_L_PCStopSound();

            SD_PlayDigitized(sd_digiMap[sound],lp,rp);
            sd_soundPositioned = ispos;
            sd_soundNumber = sound;
            sd_soundPriority = s->priority;
#else
            return 0;
#endif
        }
        else
        {
#ifdef NOTYET
            if (s->priority < sd_digiPriority)
                return(false);
#endif

            int channel = SD_PlayDigitized(sd_digiMap[sound], lp, rp);
            sd_soundPositioned = ispos;
            sd_digiNumber = sound;
            sd_digiPriority = s->priority;
            return channel + 1;
        }

        return(true);
    }

    if (sd_soundMode == sdm_Off)
        return 0;

    if (!s->length)
        throw Exception("Sound::Play() - Zero length sound");
    if (s->priority < sd_soundPriority)
        return 0;

    switch (sd_soundMode)
    {
        case sdm_PC:
//            SD_L_PCPlaySound((PCSound *)s);
            break;
        case sdm_AdLib:
            SD_L_ALPlaySound((AdLibSound *)s);
            break;
		default:
			;
    }

    sd_soundNumber = sound;
    sd_soundPriority = s->priority;

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
//      SD_SoundPlaying() - returns the sound number that's playing, or 0 if
//              no sound is playing
//
////////////////////////////////////////////////////////////////////////////////
word SD_SoundPlaying()
{
    Boolean8 result = false;

    switch (sd_soundMode)
    {
        case sdm_PC:
            result = pcSound? true : false;
            break;
        case sdm_AdLib:
            result = sd_alSound? true : false;
            break;
		default:
			;
    }

    if (result)
        return(sd_soundNumber);
    else
        return(false);
}

////////////////////////////////////////////////////////////////////////////////
//
//      SD_StopSound() - if a sound is playing, stops it
//
////////////////////////////////////////////////////////////////////////////////
void Sound::Stop()
{
    if (sd_digiPlaying)
        Sound::StopDigitized();

    switch (sd_soundMode)
    {
        case sdm_PC:
//            SD_L_PCStopSound();
            break;
        case sdm_AdLib:
            SD_L_ALStopSound();
            break;
		default:
			;
    }

    sd_soundPositioned = false;

    SD_L_SoundFinished();
}

////////////////////////////////////////////////////////////////////////////////
//
//      SD_WaitSoundDone() - waits until the current sound is done playing
//
////////////////////////////////////////////////////////////////////////////////
void SD_WaitSoundDone()
{
    while (SD_SoundPlaying())
        I_Delay(5);
}

////////////////////////////////////////////////////////////////////////////////
//
//      SD_MusicOn() - turns on the sequencer
//
////////////////////////////////////////////////////////////////////////////////
void Sound::MusicOn()
{
    sd_sqActive = true;
}

////////////////////////////////////////////////////////////////////////////////
//
//      SD_MusicOff() - turns off the sequencer and any playing notes
//      returns the last music offset for music continue
//
////////////////////////////////////////////////////////////////////////////////
int Sound::MusicOff()
{
    word    i;

    sd_sqActive = false;
    switch (sd_musicMode)
    {
        case smm_AdLib:
            alOut(alEffects, 0);
            for (i = 0;i < sqMaxTracks;i++)
                alOut(alFreqH + i + 1, 0);
            break;
		default:
			;
    }

    return (int) (sd_sqHackPtr-sd_sqHack);
}

////////////////////////////////////////////////////////////////////////////////
//
//      SD_StartMusic() - starts playing the music pointed to
//
////////////////////////////////////////////////////////////////////////////////
void SD_StartMusic(int chunk)
{
    Sound::MusicOff();

    if (sd_musicMode == smm_AdLib)
    {
        int32_t chunkLen = audioSegs.cacheChunk(chunk);
        sd_sqHack = (const word *)(const void *) audioSegs[chunk];     // alignment is correct
        if(*sd_sqHack == 0) sd_sqHackLen = sd_sqHackSeqLen = chunkLen;
        else sd_sqHackLen = sd_sqHackSeqLen = *sd_sqHack++;
        sd_sqHackPtr = sd_sqHack;
        sd_sqHackTime = 0;
        sd_alTimeCount = 0;
        Sound::MusicOn();
    }
}

void SD_ContinueMusic(int chunk, int startoffs)
{
    Sound::MusicOff();

    if (sd_musicMode == smm_AdLib)
    {
        int32_t chunkLen = audioSegs.cacheChunk(chunk);
        sd_sqHack = (const word *)(const void *) audioSegs[chunk];     // alignment is correct
        if(*sd_sqHack == 0) sd_sqHackLen = sd_sqHackSeqLen = chunkLen;
        else sd_sqHackLen = sd_sqHackSeqLen = *sd_sqHack++;
        sd_sqHackPtr = sd_sqHack;

        if(startoffs >= sd_sqHackLen)
        {
            throw Exception("SD_StartMusic: Illegal startoffs provided!");
        }

        // fast forward to correct position
        // (needed to reconstruct the instruments)

        for(int i = 0; i < startoffs; i += 2)
        {
            byte reg = *(byte *)sd_sqHackPtr;
            byte val = *(((byte *)sd_sqHackPtr) + 1);
            if(reg >= 0xb1 && reg <= 0xb8) val &= 0xdf;           // disable play note flag
            else if(reg == 0xbd) val &= 0xe0;                     // disable drum flags

            alOut(reg,val);
            sd_sqHackPtr += 2;
            sd_sqHackLen -= 4;
        }
        sd_sqHackTime = 0;
        sd_alTimeCount = 0;

        Sound::MusicOn();
    }
}

////////////////////////////////////////////////////////////////////////////////
//
//      SD_FadeOutMusic() - starts fading out the music. Call 
// SD_MusicPlaying()
//              to see if the fadeout is complete
//
////////////////////////////////////////////////////////////////////////////////
void SD_FadeOutMusic()
{
    switch (sd_musicMode)
    {
        case smm_AdLib:
            // DEBUG - quick hack to turn the music off
            Sound::MusicOff();
            break;
		default:
			;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
//      SD_MusicPlaying() - returns true if music is currently playing, 
// false if
//              not
//
////////////////////////////////////////////////////////////////////////////////
Boolean8 SD_MusicPlaying()
{
    Boolean8 result;

    switch (sd_musicMode)
    {
        case smm_AdLib:
            result = sd_sqActive;
            break;
        default:
            result = false;
            break;
    }

    return(result);
}
