#ifndef __FMOPL_H_
#define __FMOPL_H_

#define HAS_YM3812 1

/* --- select emulation chips --- */
#define BUILD_YM3812 (HAS_YM3812)
#define BUILD_YM3526 (HAS_YM3526)
#define BUILD_Y8950  (HAS_Y8950)

/* select output bits size of output : 8 or 16 */
#define OPL_SAMPLE_BITS 16

/* compiler dependence */
#ifndef OSD_CPU_H
#define OSD_CPU_H
typedef unsigned char  UINT8;   /* unsigned  8bit */
typedef unsigned short UINT16;  /* unsigned 16bit */
typedef unsigned int   UINT32;  /* unsigned 32bit */
typedef signed char    INT8;    /* signed  8bit   */
typedef signed short   INT16;   /* signed 16bit   */
typedef signed int     INT32;   /* signed 32bit   */
#endif

#if (OPL_SAMPLE_BITS==16)
typedef INT16 OPLSAMPLE;
#endif
#if (OPL_SAMPLE_BITS==8)
typedef INT8 OPLSAMPLE;
#endif


typedef void (*OPL_TIMERHANDLER)(int channel,double interval_Sec);
typedef void (*OPL_IRQHANDLER)(int param,int irq);
typedef void (*OPL_UPDATEHANDLER)(int param,int min_interval_us);
typedef void (*OPL_PORTHANDLER_W)(int param,unsigned char data);
typedef unsigned char (*OPL_PORTHANDLER_R)(int param);


#if BUILD_YM3812

int  YM3812Init(int num, int clock, int rate);
void YM3812Shutdown(void);
void YM3812ResetChip(int which);
int  YM3812Write(int which, int a, int v);
unsigned char YM3812Read(int which, int a);
int  YM3812TimerOver(int which, int c);
void YM3812UpdateOne(int which, INT16 *buffer, int length);

void YM3812SetTimerHandler(int which, OPL_TIMERHANDLER TimerHandler, int channelOffset);
void YM3812SetIRQHandler(int which, OPL_IRQHANDLER IRQHandler, int param);
void YM3812SetUpdateHandler(int which, OPL_UPDATEHANDLER UpdateHandler, int param);

INT16 Amp( INT16 );

#endif

#endif
