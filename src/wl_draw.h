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
extern  short *vid_pixelangle;
extern  int32_t finetangent[FINEANGLES/4];
extern  fixed sintable[];
extern  fixed *costable;
extern  int *vid_wallheight;
extern  word horizwall[],vertwall[];
extern  int32_t    lasttimecount;
extern  int32_t    frameon;

extern  unsigned screenloc[3];

extern  Boolean fizzlein, fpscounter;

extern  fixed   viewx,viewy;                    // the focal point
extern  fixed   viewsin,viewcos;

void    ThreeDRefresh ();
void    CalcTics ();

#pragma pack(push, 1)
struct t_compshape
{
    word leftpix,rightpix;
    word dataofs[64];
// table data after dataofs[rightpix-leftpix+1]
};
#pragma pack(pop)

// IOANCH 20130724: delocalized vgaCeiling
extern byte vgaCeiling_wl6[];
extern byte vgaCeiling_sod[];

// IOANCH: defined floor colour
#define FLOOR_COLOUR 0x19

#endif

