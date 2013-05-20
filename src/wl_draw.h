/*
 =============================================================================
 
 WL_DRAW DEFINITIONS
 
 =============================================================================
 */

#ifndef WL_DRAW_H_
#define WL_DRAW_H_


//
// math tables
//
extern  short *pixelangle;
extern  int32_t finetangent[FINEANGLES/4];
extern  fixed sintable[];
extern  fixed *costable;
extern  int *wallheight;
extern  word horizwall[],vertwall[];
extern  int32_t    lasttimecount;
extern  int32_t    frameon;

extern  unsigned screenloc[3];

extern  Boolean fizzlein, fpscounter;

extern  fixed   viewx,viewy;                    // the focal point
extern  fixed   viewsin,viewcos;

void    ThreeDRefresh ();
void    CalcTics ();

typedef struct
{
    word leftpix,rightpix;
    word dataofs[64];
// table data after dataofs[rightpix-leftpix+1]
} t_compshape;


#endif

