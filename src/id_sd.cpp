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
//                      SoundBlasterPresent - SoundBlaster card present?
//                      AdLibPresent - AdLib card present?
//                      SoundMode - What device is used for sound effects
//                              (Use SM_SetSoundMode() to set)
//                      MusicMode - What device is used for music
//                              (Use SM_SetMusicMode() to set)
//                      DigiMode - What device is used for digitized sound effects
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
#include "wl_main.h"
#include <SDL_mixer.h>
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

typedef struct
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
} headchunk;

typedef struct
{
	char chunkid[4];
	longword chunklength;
} wavechunk;

typedef struct
{
    uint32_t startpage;
    uint32_t length;
} digiinfo;

// IOANCH 20130301: unification
static Mix_Chunk *SoundChunks[STARTMUSIC_wl6 - STARTDIGISOUNDS_wl6 > 
							  STARTMUSIC_sod - STARTDIGISOUNDS_sod ?
							  STARTMUSIC_wl6 - STARTDIGISOUNDS_wl6 :
							  STARTMUSIC_sod - STARTDIGISOUNDS_sod];
static byte      *SoundBuffers[STARTMUSIC_wl6 - STARTDIGISOUNDS_wl6 > 
							   STARTMUSIC_sod - STARTDIGISOUNDS_sod ?
							   STARTMUSIC_wl6 - STARTDIGISOUNDS_wl6 :
							   STARTMUSIC_sod - STARTDIGISOUNDS_sod];

globalsoundpos channelSoundPos[MIX_CHANNELS];

//      Global variables
        Boolean         AdLibPresent,
                        SoundBlasterPresent,SBProPresent,
                        SoundPositioned;
        SDMode          SoundMode;
        SMMode          MusicMode;
        SDSMode         DigiMode;
static  byte          **SoundTable;

// IOANCH 20130301: unification
int             DigiMap[(unsigned int)LASTSOUND_wl6 > 
						(unsigned int)LASTSOUND_sod ? 
						(unsigned int)LASTSOUND_wl6 : 
						(unsigned int)LASTSOUND_sod];
int DigiChannel[STARTMUSIC_wl6 - STARTDIGISOUNDS_wl6 > STARTMUSIC_sod - 
				STARTDIGISOUNDS_sod ? STARTMUSIC_wl6 - STARTDIGISOUNDS_wl6 :
				STARTMUSIC_sod - STARTDIGISOUNDS_sod];

//      Internal variables
static  Boolean                 SD_Started;
static  Boolean                 nextsoundpos;
// IOANCH 20130301: unification
static  unsigned int            SoundNumber;
static  unsigned int            DigiNumber;
static  word                    SoundPriority;
static  word                    DigiPriority;
static  int                     LeftPosition;
static  int                     RightPosition;

        word                    NumDigi;
static  digiinfo               *DigiList;
static  Boolean                 DigiPlaying;

//      PC Sound variables
// static  volatile byte           pcLastSample;
static  byte * volatile         pcSound;
// IOANCH 11.06.2012: commented to made compiler stop complaining
//static  longword                pcLengthLeft;

//      AdLib variables
static  byte * volatile         alSound;
static  byte                    alBlock;
static  longword                alLengthLeft;
static  longword                alTimeCount;
static  Instrument              alZeroInst;

//      Sequencer variables
static  volatile Boolean        sqActive;
static  word                   *sqHack;
static  word                   *sqHackPtr;
static  int                     sqHackLen;
static  int                     sqHackSeqLen;
static  longword                sqHackTime;

#ifdef USE_GPL

DBOPL::Chip oplChip;

//
// YM3812Init
//
static inline bool YM3812Init(int numChips, int clock, int rate)
{
	oplChip.Setup(rate);
	return false;
}

//
// YM3812Write
//
static inline void YM3812Write(DBOPL::Chip &which, Bit32u reg, Bit8u val)
{
	which.WriteReg(reg, val);
}

//
// YM3812UpdateOne
//
static inline void YM3812UpdateOne(DBOPL::Chip &which, int16_t *stream, int length)
{
	Bit32s buffer[512 * 2];
	int i;

	// length is at maximum samplesPerMusicTick = param_samplerate / 700
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

static const int oplChip = 0;

#endif

//
// SD_L_SoundFinished
//
static void SD_L_SoundFinished()
{
    // IOANCH 20130301: unification
	SoundNumber   = 0;
	SoundPriority = 0;
}


#ifdef NOTYET

void SD_L_turnOnPCSpeaker(word timerval);
#pragma aux SD_L_turnOnPCSpeaker = \
        "mov    al,0b6h" \
        "out    43h,al" \
        "mov    al,bl" \
        "out    42h,al" \
        "mov    al,bh" \
        "out    42h,al" \
        "in     al,61h" \
        "or     al,3"   \
        "out    61h,al" \
        parm [bx] \
        modify exact [al]

void SD_L_turnOffPCSpeaker();
#pragma aux SD_L_turnOffPCSpeaker = \
        "in     al,61h" \
        "and    al,0fch" \
        "out    61h,al" \
        modify exact [al]

void SD_L_setPCSpeaker(byte val);
#pragma aux SD_L_setPCSpeaker = \
        "in     al,61h" \
        "and    al,0fch" \
        "or     al,ah" \
        "out    61h,al" \
        parm [ah] \
        modify exact [al]

void inline SD_L_DoFX()
{
        if(pcSound)
        {
                if(*pcSound!=pcLastSample)
                {
                        pcLastSample=*pcSound;

                        if(pcLastSample)
                                SD_L_turnOnPCSpeaker(pcLastSample*60);
                        else
                                SD_L_turnOffPCSpeaker();
                }
                pcSound++;
                pcLengthLeft--;
                if(!pcLengthLeft)
                {
                        pcSound=0;
                        // IOANCH 20130301: unification
                        SoundNumber=0;
                        SoundPriority=0;
                        SD_L_turnOffPCSpeaker();
                }
        }

        // [adlib sound stuff removed...]
}

static void SD_L_DigitizedDoneInIRQ();

void inline SD_L_DoFast()
{
        count_fx++;
        if(count_fx>=5)
        {
                count_fx=0;

                SD_L_DoFX();

                count_time++;
                if(count_time>=2)
                {
                        TimeCount++;
                        count_time=0;
                }
        }

        // [adlib music and soundsource stuff removed...]

        TimerCount+=TimerDivisor;
        if(*((word *)&TimerCount+1))
        {
                *((word *)&TimerCount+1)=0;
                t0OldService();
        }
        else
        {
                outp(0x20,0x20);
        }
}

// Timer 0 ISR for 7000Hz interrupts
void __interrupt SD_L_t0ExtremeAsmService()
{
        if(pcindicate)
        {
                if(pcSound)
                {
                        SD_L_setPCSpeaker(((*pcSound++)&0x80)>>6);
                        pcLengthLeft--;
                        if(!pcLengthLeft)
                        {
                                pcSound=0;
                                SD_L_turnOffPCSpeaker();
                                SD_L_DigitizedDoneInIRQ();
                        }
                }
        }
        extreme++;
        if(extreme>=10)
        {
                extreme=0;
                SD_L_DoFast();
        }
        else
                outp(0x20,0x20);
}

// Timer 0 ISR for 700Hz interrupts
void __interrupt SD_L_t0FastAsmService()
{
        SD_L_DoFast();
}

// Timer 0 ISR for 140Hz interrupts
void __interrupt SD_L_t0SlowAsmService()
{
        count_time++;
        if(count_time>=2)
        {
                TimeCount++;
                count_time=0;
        }

        SD_L_DoFX();

        TimerCount+=TimerDivisor;
        if(*((word *)&TimerCount+1))
        {
                *((word *)&TimerCount+1)=0;
                t0OldService();
        }
        else
                outp(0x20,0x20);
}

void SD_L_IndicatePC(Boolean ind)
{
        pcindicate=ind;
}

////////////////////////////////////////////////////////////////////////////////
//
//      SD_L_SetTimer0() - Sets system timer 0 to the specified speed
//
////////////////////////////////////////////////////////////////////////////////
static void
SD_L_SetTimer0(word speed)
{
#ifndef TPROF   // If using Borland's profiling, don't screw with the timer
//      _asm pushfd
        _asm cli

        outp(0x43,0x36);                                // Change timer 0
        outp(0x40,(byte)speed);
        outp(0x40,speed >> 8);
        // Kludge to handle special case for digitized PC sounds
        if (TimerDivisor == (1192030 / (TickBase * 100)))
                TimerDivisor = (1192030 / (TickBase * 10));
        else
                TimerDivisor = speed;

//      _asm popfd
        _asm    sti
#else
        TimerDivisor = 0x10000;
#endif
}

////////////////////////////////////////////////////////////////////////////////
//
//      SD_L_SetIntsPerSec() - Uses SD_L_SetTimer0() to set the number of
//              interrupts generated by system timer 0 per second
//
////////////////////////////////////////////////////////////////////////////////

//
// SD_L_SetIntsPerSec
//
static void
SD_L_SetIntsPerSec(word ints)
{
        TimerRate = ints;
        SD_L_SetTimer0(1192030 / ints);
}

//
// SD_L_SetTimerSpeed
//
static void
SD_L_SetTimerSpeed()
{
        word    rate;
        void (_interrupt *isr)();

        if ((DigiMode == sds_PC) && DigiPlaying)
        {
                rate = TickBase * 100;
                isr = SD_L_t0ExtremeAsmService;
        }
        else if ((MusicMode == smm_AdLib) || ((DigiMode == sds_SoundSource) && DigiPlaying)     )
        {
                rate = TickBase * 10;
                isr = SD_L_t0FastAsmService;
        }
        else
        {
                rate = TickBase * 2;
                isr = SD_L_t0SlowAsmService;
        }

        if (rate != TimerRate)
        {
                _dos_setvect(8,isr);
                SD_L_SetIntsPerSec(rate);
                TimerRate = rate;
        }
}

//
//      PC Sound code
//

////////////////////////////////////////////////////////////////////////////////
//
//      SD_L_PCPlaySample() - Plays the specified sample on the PC speaker
//
////////////////////////////////////////////////////////////////////////////////
#ifdef  _MUSE_
void
#else
static void
#endif
SD_L_PCPlaySample(byte *data,longword len,Boolean inIRQ)
{
        if(!inIRQ)
        {
//              _asm    pushfd
                _asm    cli
        }

        SD_L_IndicatePC(true);

        pcLengthLeft = len;
        pcSound = (volatile byte *)data;

        if(!inIRQ)
        {
//              _asm    popfd
                _asm    sti
        }
}

////////////////////////////////////////////////////////////////////////////////
//
//      SD_L_PCStopSample() - Stops a sample playing on the PC speaker
//
////////////////////////////////////////////////////////////////////////////////
#ifdef  _MUSE_
void
#else
static void
#endif
SD_L_PCStopSampleInIRQ()
{
        pcSound = 0;

        SD_L_IndicatePC(false);

        _asm    in      al,0x61                 // Turn the speaker off
        _asm    and     al,0xfd                 // ~2
        _asm    out     0x61,al
}

////////////////////////////////////////////////////////////////////////////////
//
//      SD_L_PCPlaySound() - Plays the specified sound on the PC speaker
//
////////////////////////////////////////////////////////////////////////////////
#ifdef  _MUSE_
void
#else
static void
#endif
SD_L_PCPlaySound(PCSound *sound)
{
//      _asm    pushfd
        _asm    cli

        pcLastSample = -1;
        pcLengthLeft = sound->common.length;
        pcSound = sound->data;

//      _asm    popfd
        _asm    sti
}

////////////////////////////////////////////////////////////////////////////////
//
//      SD_L_PCStopSound() - Stops the current sound playing on the PC Speaker
//
////////////////////////////////////////////////////////////////////////////////
#ifdef  _MUSE_
void
#else
static void
#endif
SD_L_PCStopSound()
{
//      _asm    pushfd
        _asm    cli

        pcSound = 0;

        _asm    in      al,0x61                 // Turn the speaker off
        _asm    and     al,0xfd                 // ~2
        _asm    out     0x61,al

//      _asm    popfd
        _asm    sti
}

////////////////////////////////////////////////////////////////////////////////
//
//      SD_L_ShutPC() - Turns off the pc speaker
//
////////////////////////////////////////////////////////////////////////////////
static void
SD_L_ShutPC()
{
//      _asm    pushfd
        _asm    cli

        pcSound = 0;

        _asm    in      al,0x61                 // Turn the speaker & gate off
        _asm    and     al,0xfc                 // ~3
        _asm    out     0x61,al

//      _asm    popfd
        _asm    sti
}

#endif

//
// SD_StopDigitized
//
void SD_StopDigitized()
{
    DigiPlaying = false;
    // IOANCH 20130301: unification
    DigiNumber = 0;
    DigiPriority = 0;
    SoundPositioned = false;
    if ((DigiMode == sds_PC) && (SoundMode == sdm_PC))
        SD_L_SoundFinished();

    switch (DigiMode)
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
    if(DigiChannel[which] != -1) return DigiChannel[which];

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
        Quit("SD_SetPosition: Illegal position");

    switch (DigiMode)
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
Sint16 GetSample(float csample, byte *samples, int size)
{
    float s0=0, s1=0, s2=0;
    int cursample = (int) csample;
    float sf = csample - (float) cursample;

    if(cursample-1 >= 0) s0 = (float) (samples[cursample-1] - 128);
    s1 = (float) (samples[cursample] - 128);
    if(cursample+1 < size) s2 = (float) (samples[cursample+1] - 128);

    float val = s0*sf*(sf-1)/2 - s1*(sf*sf-1) + s2*(sf+1)*sf/2;
    int32_t intval = (int32_t) (val * 256);
    if(intval < -32768) intval = -32768;
    else if(intval > 32767) intval = 32767;
    return (Sint16) intval;
}

//
// SD_PrepareSound
//
void SD_PrepareSound(int which)
{
    if(DigiList == NULL)
        Quit("SD_PrepareSound(%i): DigiList not initialized!\n", which);

    int page = DigiList[which].startpage;
    int size = DigiList[which].length;

    byte *origsamples = PM_GetSound(page);
    if(origsamples + size >= PM_GetEnd())
        Quit("SD_PrepareSound(%i): Sound reaches out of page file!\n", which);

    int destsamples = (int) ((float) size * (float) Config::SampleRate()
        / (float) ORIGSAMPLERATE);

    byte *wavebuffer = (byte *) malloc(sizeof(headchunk) + sizeof(wavechunk)
        + destsamples * 2);     // dest are 16-bit samples
    if(wavebuffer == NULL)
        Quit("Unable to allocate wave buffer for sound %i!\n", which);

    headchunk head = {{'R','I','F','F'}, 0, {'W','A','V','E'},
        {'f','m','t',' '}, 0x10, 0x0001, 1, static_cast<longword>(Config::SampleRate()),
        static_cast<longword>(Config::SampleRate()*2), 2, 16};
    wavechunk dhead = {{'d', 'a', 't', 'a'}, static_cast<longword>(destsamples*2)};
    head.filelenminus8 = sizeof(head) + destsamples*2;  // (sizeof(dhead)-8 = 0)
    memcpy(wavebuffer, &head, sizeof(head));
    memcpy(wavebuffer+sizeof(head), &dhead, sizeof(dhead));

    // alignment is correct, as wavebuffer comes from malloc
    // and sizeof(headchunk) % 4 == 0 and sizeof(wavechunk) % 4 == 0
    Sint16 *newsamples = (Sint16 *)(void *) (wavebuffer + sizeof(headchunk)
        + sizeof(wavechunk));
    float cursample = 0.F;
    float samplestep = (float) ORIGSAMPLERATE / (float) Config::SampleRate();
    for(int i=0; i<destsamples; i++, cursample+=samplestep)
    {
        newsamples[i] = GetSample((float)size * (float)i / (float)destsamples,
            origsamples, size);
    }
    SoundBuffers[which] = wavebuffer;

    SoundChunks[which] = Mix_LoadWAV_RW(SDL_RWFromMem(wavebuffer,
        sizeof(headchunk) + sizeof(wavechunk) + destsamples * 2), 1);
}

//
// SD_PlayDigitized
//
int SD_PlayDigitized(word which,int leftpos,int rightpos)
{
    if (!DigiMode)
        return 0;

    if (which >= NumDigi)
        Quit("SD_PlayDigitized: bad sound number %i", which);

    int channel = SD_GetChannelForDigi(which);
    SD_SetPosition(channel, leftpos,rightpos);

    DigiPlaying = true;

    Mix_Chunk *sample = SoundChunks[which];
    if(sample == NULL)
    {
        printf("SoundChunks[%i] is NULL!\n", which);
        return 0;
    }

    if(Mix_PlayChannel(channel, sample, 0) == -1)
    {
        printf("Unable to play sound: %s\n", Mix_GetError());
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
    Boolean devicenotpresent;

    if (mode == DigiMode)
        return;

    SD_StopDigitized();

    devicenotpresent = false;
    switch (mode)
    {
        case sds_SoundBlaster:
            if (!SoundBlasterPresent)
                devicenotpresent = true;
            break;
		default:
			;
    }

    if (!devicenotpresent)
    {
        DigiMode = mode;

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
    word *soundInfoPage = (word *) (void *) PM_GetPage(ChunksInFile-1);
    NumDigi = (word) PM_GetPageSize(ChunksInFile - 1) / 4;

    DigiList = (digiinfo *) malloc(NumDigi * sizeof(digiinfo));
    int i;
    for(i = 0; i < NumDigi; i++)
    {
        // Calculate the size of the digi from the sizes of the pages between
        // the start page and the start page of the next sound

        DigiList[i].startpage = soundInfoPage[i * 2];
        if((int) DigiList[i].startpage >= ChunksInFile - 1)
        {
            NumDigi = i;
            break;
        }

        int lastPage;
        if(i < NumDigi - 1)
        {
            lastPage = soundInfoPage[i * 2 + 2];
            if(lastPage == 0 || lastPage + PMSoundStart > ChunksInFile - 1) lastPage = ChunksInFile - 1;
            else lastPage += PMSoundStart;
        }
        else lastPage = ChunksInFile - 1;

        int size = 0;
        for(int page = PMSoundStart + DigiList[i].startpage; page < lastPage; page++)
            size += PM_GetPageSize(page);

        // Don't include padding of sound info page, if padding was added
        if(lastPage == ChunksInFile - 1 && PMSoundInfoPagePadded) size--;

        // Patch lower 16-bit of size with size from sound info page.
        // The original VSWAP contains padding which is included in the page size,
        // but not included in the 16-bit size. So we use the more precise value.
        if((size & 0xffff0000) != 0 && (size & 0xffff) < soundInfoPage[i * 2 + 1])
            size -= 0x10000;
        size = (size & 0xffff0000) | soundInfoPage[i * 2 + 1];

        DigiList[i].length = size;
    }

	// IOANCH 20130301: unification
	unsigned int LASTSOUND_max = IMPALE((unsigned int)LASTSOUND);
    for(i = 0; i < (signed int)LASTSOUND_max; i++)
    {
        DigiMap[i] = -1;
        DigiChannel[i] = -1;
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
    alSound = 0;
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

    alLengthLeft = sound->common.length;
    data = sound->data;
    alBlock = ((sound->block & 7) << 2) | 0x20;
    inst = &sound->inst;

    if (!(inst->mSus | inst->cSus))
    {
        Quit("SD_L_ALPlaySound() - Bad instrument");
    }

    SD_L_AlSetFXInst(inst);
    alSound = (byte *)data;
}

//
// SD_L_ShutAL
//
// Shuts down the AdLib card for sound effects
//
static void SD_L_ShutAL()
{
    alSound = 0;
    alOut(alEffects,0);
    alOut(alFreqH + 0,0);
    SD_L_AlSetFXInst(&alZeroInst);
}

//
// SD_L_CleanAL
//
// Totally shuts down the AdLib card
//
static void SD_L_CleanAL()
{
    int     i;

    alOut(alEffects,0);
    for (i = 1; i < 0xf5; i++)
        alOut(i, 0);
}

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

//
// SD_L_DetectAdLib 
//             
// Determines if there's an AdLib (or SoundBlaster emulating an AdLib) present
//
static Boolean SD_L_DetectAdLib()
{
    for (int i = 1; i <= 0xf5; i++)       // Zero all the registers
        alOut(i, 0);

    alOut(1, 0x20);             // Set WSE=1
//    alOut(8, 0);                // Set CSM=0 & SEL=0

    return true;
}

//
// SD_L_ShutDevice 
//
// turns off whatever device was being used for sound fx
//
static void SD_L_ShutDevice()
{
    switch (SoundMode)
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
    SoundMode = sdm_Off;
}

////////////////////////////////////////////////////////////////////////////////
//
//      SD_L_CleanDevice() - totally shuts down all sound devices
//
////////////////////////////////////////////////////////////////////////////////
static void SD_L_CleanDevice()
{
    if ((SoundMode == sdm_AdLib) || (MusicMode == smm_AdLib))
        SD_L_CleanAL();
}

////////////////////////////////////////////////////////////////////////////////
//
//      SD_L_StartDevice() - turns on whatever device is to be used for 
// sound fx
//
////////////////////////////////////////////////////////////////////////////////
static void SD_L_StartDevice()
{
    switch (SoundMode)
    {
        case sdm_AdLib:
            SD_L_StartAL();
            break;
		default:
			;
    }
    // IOANCH 20130301: unification
    SoundNumber = 0;
    SoundPriority = 0;
}

//      Public routines

////////////////////////////////////////////////////////////////////////////////
//
//      SD_SetSoundMode() - Sets which sound hardware to use for sound 
// effects
//
////////////////////////////////////////////////////////////////////////////////
Boolean SD_SetSoundMode(SDMode mode)
{
    Boolean result = false;
    word    tableoffset;

    SD_StopSound();

    if ((mode == sdm_AdLib) && !AdLibPresent)
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
            if (AdLibPresent)
                result = true;
            break;
        default:
            Quit("SD_SetSoundMode: Invalid sound mode %i", mode);
            return false;
    }
    SoundTable = &audiosegs[tableoffset];

    if (result && (mode != SoundMode))
    {
        SD_L_ShutDevice();
        SoundMode = mode;
        SD_L_StartDevice();
    }

    return(result);
}

////////////////////////////////////////////////////////////////////////////////
//
//      SD_SetMusicMode() - sets the device to use for background music
//
////////////////////////////////////////////////////////////////////////////////
Boolean SD_SetMusicMode(SMMode mode)
{
    Boolean result = false;

    SD_FadeOutMusic();
    while (SD_MusicPlaying())
        I_Delay(5);

    switch (mode)
    {
        case smm_Off:
            result = true;
            break;
        case smm_AdLib:
            if (AdLibPresent)
                result = true;
            break;
    }

    if (result)
        MusicMode = mode;

//    SD_L_SetTimerSpeed();

    return(result);
}

int numreadysamples = 0;
byte *curAlSound = 0;
byte *curAlSoundPtr = 0;
longword curAlLengthLeft = 0;
int soundTimeCounter = 5;
int samplesPerMusicTick;

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
        if(numreadysamples)
        {
            if(numreadysamples<sampleslen)
            {
                YM3812UpdateOne(oplChip, stream16, numreadysamples);
                stream16 += numreadysamples*2;
                sampleslen -= numreadysamples;
            }
            else
            {
                YM3812UpdateOne(oplChip, stream16, sampleslen);
                numreadysamples -= sampleslen;
                return;
            }
        }
        soundTimeCounter--;
        if(!soundTimeCounter)
        {
            soundTimeCounter = 5;
            if(curAlSound != alSound)
            {
                curAlSound = curAlSoundPtr = alSound;
                curAlLengthLeft = alLengthLeft;
            }
            if(curAlSound)
            {
                if(*curAlSoundPtr)
                {
                    alOut(alFreqL, *curAlSoundPtr);
                    alOut(alFreqH, alBlock);
                }
                else alOut(alFreqH, 0);
                curAlSoundPtr++;
                curAlLengthLeft--;
                if(!curAlLengthLeft)
                {
                    curAlSound = alSound = 0;
                    // IOANCH 20130301: unification
                    SoundNumber = 0;
                    SoundPriority = 0;
                    alOut(alFreqH, 0);
                }
            }
        }
        if(sqActive)
        {
            do
            {
                if(sqHackTime > alTimeCount) break;
                sqHackTime = alTimeCount + *(sqHackPtr+1);
                alOut(*(byte *) sqHackPtr, *(((byte *) sqHackPtr)+1));
                sqHackPtr += 2;
                sqHackLen -= 4;
            }
            while(sqHackLen>0);
            alTimeCount++;
            if(!sqHackLen)
            {
                sqHackPtr = sqHack;
                sqHackLen = sqHackSeqLen;
                sqHackTime = 0;
                alTimeCount = 0;
            }
        }
        numreadysamples = samplesPerMusicTick;
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

    if(Mix_OpenAudio(Config::SampleRate(), AUDIO_S16, 2, Config::AudioBuffer()))
    {
        printf("Unable to open audio: %s\n", Mix_GetError());
        return;
    }

    Mix_ReserveChannels(2);  // reserve player and boss weapon channels
    Mix_GroupChannels(2, MIX_CHANNELS-1, 1); // group remaining channels

    // Init music

    samplesPerMusicTick = Config::SampleRate() / 700;    // SD_L_t0FastAsmService played at 700Hz

    if(YM3812Init(1,3579545,Config::SampleRate()))
    {
        printf("Unable to create virtual OPL!!\n");
    }

    for(i=1;i<0xf6;i++)
        YM3812Write(oplChip,i,0);

    YM3812Write(oplChip,1,0x20); // Set WSE=1
//    YM3812Write(0,8,0); // Set CSM=0 & SEL=0		 // already set in for statement

    Mix_HookMusic(SD_L_IMFMusicPlayer, 0);
    Mix_ChannelFinished(SD_ChannelFinished);
    AdLibPresent = true;
    SoundBlasterPresent = true;

    alTimeCount = 0;

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

    SD_MusicOff();
    SD_StopSound();

	unsigned int lastvalue = SPEAR() ? STARTMUSIC_sod - STARTDIGISOUNDS_sod :
    STARTMUSIC_wl6 - STARTDIGISOUNDS_wl6;
    
    for(int i = 0; i < (signed int)lastvalue; i++)
    {
        if(SoundChunks[i]) Mix_FreeChunk(SoundChunks[i]);
        if(SoundBuffers[i]) free(SoundBuffers[i]);
    }

    free(DigiList);

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
    LeftPosition = leftvol;
    RightPosition = rightvol;
    nextsoundpos = true;
}

////////////////////////////////////////////////////////////////////////////////
//
//      SD_PlaySound() - plays the specified sound on the appropriate 
// hardware
//
////////////////////////////////////////////////////////////////////////////////
Boolean SD_PlaySound(soundnames sound_abstract)
{
    Boolean         ispos;
    SoundCommon     *s;
    int             lp,rp;
	
	// IOANCH 20130301: abstract sound
	unsigned int sound = SPEAR.sd(sound_abstract);

    lp = LeftPosition;
    rp = RightPosition;
    LeftPosition = 0;
    RightPosition = 0;

    ispos = nextsoundpos;
    nextsoundpos = false;

    if (sound == -1 || (DigiMode == sds_Off && SoundMode == sdm_Off))
        return 0;

    s = (SoundCommon *) SoundTable[sound];

    if ((SoundMode != sdm_Off) && !s)
            Quit("SD_PlaySound() - Uncached sound");

    if ((DigiMode != sds_Off) && (DigiMap[sound] != -1))
    {
        if ((DigiMode == sds_PC) && (SoundMode == sdm_PC))
        {
#ifdef NOTYET
            if (s->priority < SoundPriority)
                return 0;

            SD_L_PCStopSound();

            SD_PlayDigitized(DigiMap[sound],lp,rp);
            SoundPositioned = ispos;
            SoundNumber = sound;
            SoundPriority = s->priority;
#else
            return 0;
#endif
        }
        else
        {
#ifdef NOTYET
            if (s->priority < DigiPriority)
                return(false);
#endif

            int channel = SD_PlayDigitized(DigiMap[sound], lp, rp);
            SoundPositioned = ispos;
            DigiNumber = sound;
            DigiPriority = s->priority;
            return channel + 1;
        }

        return(true);
    }

    if (SoundMode == sdm_Off)
        return 0;

    if (!s->length)
        Quit("SD_PlaySound() - Zero length sound");
    if (s->priority < SoundPriority)
        return 0;

    switch (SoundMode)
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

    SoundNumber = sound;
    SoundPriority = s->priority;

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
    Boolean result = false;

    switch (SoundMode)
    {
        case sdm_PC:
            result = pcSound? true : false;
            break;
        case sdm_AdLib:
            result = alSound? true : false;
            break;
		default:
			;
    }

    if (result)
        return(SoundNumber);
    else
        return(false);
}

////////////////////////////////////////////////////////////////////////////////
//
//      SD_StopSound() - if a sound is playing, stops it
//
////////////////////////////////////////////////////////////////////////////////
void SD_StopSound()
{
    if (DigiPlaying)
        SD_StopDigitized();

    switch (SoundMode)
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

    SoundPositioned = false;

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
void SD_MusicOn()
{
    sqActive = true;
}

////////////////////////////////////////////////////////////////////////////////
//
//      SD_MusicOff() - turns off the sequencer and any playing notes
//      returns the last music offset for music continue
//
////////////////////////////////////////////////////////////////////////////////
int SD_MusicOff()
{
    word    i;

    sqActive = false;
    switch (MusicMode)
    {
        case smm_AdLib:
            alOut(alEffects, 0);
            for (i = 0;i < sqMaxTracks;i++)
                alOut(alFreqH + i + 1, 0);
            break;
		default:
			;
    }

    return (int) (sqHackPtr-sqHack);
}

////////////////////////////////////////////////////////////////////////////////
//
//      SD_StartMusic() - starts playing the music pointed to
//
////////////////////////////////////////////////////////////////////////////////
void SD_StartMusic(int chunk)
{
    SD_MusicOff();

    if (MusicMode == smm_AdLib)
    {
        int32_t chunkLen = CA_CacheAudioChunk(chunk);
        sqHack = (word *)(void *) audiosegs[chunk];     // alignment is correct
        if(*sqHack == 0) sqHackLen = sqHackSeqLen = chunkLen;
        else sqHackLen = sqHackSeqLen = *sqHack++;
        sqHackPtr = sqHack;
        sqHackTime = 0;
        alTimeCount = 0;
        SD_MusicOn();
    }
}

void SD_ContinueMusic(int chunk, int startoffs)
{
    SD_MusicOff();

    if (MusicMode == smm_AdLib)
    {
        int32_t chunkLen = CA_CacheAudioChunk(chunk);
        sqHack = (word *)(void *) audiosegs[chunk];     // alignment is correct
        if(*sqHack == 0) sqHackLen = sqHackSeqLen = chunkLen;
        else sqHackLen = sqHackSeqLen = *sqHack++;
        sqHackPtr = sqHack;

        if(startoffs >= sqHackLen)
        {
            Quit("SD_StartMusic: Illegal startoffs provided!");
        }

        // fast forward to correct position
        // (needed to reconstruct the instruments)

        for(int i = 0; i < startoffs; i += 2)
        {
            byte reg = *(byte *)sqHackPtr;
            byte val = *(((byte *)sqHackPtr) + 1);
            if(reg >= 0xb1 && reg <= 0xb8) val &= 0xdf;           // disable play note flag
            else if(reg == 0xbd) val &= 0xe0;                     // disable drum flags

            alOut(reg,val);
            sqHackPtr += 2;
            sqHackLen -= 4;
        }
        sqHackTime = 0;
        alTimeCount = 0;

        SD_MusicOn();
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
    switch (MusicMode)
    {
        case smm_AdLib:
            // DEBUG - quick hack to turn the music off
            SD_MusicOff();
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
Boolean SD_MusicPlaying()
{
    Boolean result;

    switch (MusicMode)
    {
        case smm_AdLib:
            result = sqActive;
            break;
        default:
            result = false;
            break;
    }

    return(result);
}
