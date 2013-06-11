//
// IOAN_BAS.CPP
//
// File by Ioan Chera, started 17.05.2012
//
// Basic general setup. New functions and such placed here, instead of randomly through the exe
//

#include "WL_DEF.H"
#pragma hdrstop

//
// Basic::IsDangerous
//
// Check if the Nazi is ready to fire or firing (important for fighting SS and bosses)
//
extern statetype s_grdshoot2, s_grdshoot3;
extern statetype s_ofcshoot2, s_ofcshoot3;
extern statetype s_ssshoot2, s_ssshoot3, s_ssshoot4, s_ssshoot5, s_ssshoot6, s_ssshoot7, s_ssshoot8, s_ssshoot9;
extern statetype s_mutshoot1, s_mutshoot2, s_mutshoot3, s_mutshoot4;
extern statetype s_bossshoot1, s_bossshoot2, s_bossshoot3, s_bossshoot4, s_bossshoot5, s_bossshoot6, s_bossshoot7, s_bossshoot8;
extern statetype s_mechashoot1, s_mechashoot2, s_mechashoot3, s_mechashoot4, s_mechashoot5, s_mechashoot6;
extern statetype s_hitlershoot1, s_hitlershoot2, s_hitlershoot3, s_hitlershoot4, s_hitlershoot5, s_hitlershoot6;
extern statetype s_gretelshoot1, s_gretelshoot2, s_gretelshoot3, s_gretelshoot4, s_gretelshoot5, s_gretelshoot6, s_gretelshoot7, s_gretelshoot8;
extern statetype s_fatshoot1, s_fatshoot2, s_fatshoot3, s_fatshoot4, s_fatshoot5, s_fatshoot6;
extern statetype s_transshoot1, s_transshoot2, s_transshoot3, s_transshoot4, s_transshoot5, s_transshoot6, s_transshoot7, s_transshoot8;
extern statetype s_willshoot1, s_willshoot2, s_willshoot3, s_willshoot4, s_willshoot5, s_willshoot6;
extern statetype s_ubershoot1, s_ubershoot2, s_ubershoot3, s_ubershoot4, s_ubershoot5, s_ubershoot6, s_ubershoot7;
extern statetype s_deathshoot1, s_deathshoot2, s_deathshoot3, s_deathshoot4, s_deathshoot5;
boolean Basic_IsDamaging(objtype *ret, char dist)
{
	switch(ret->obclass)
	{
        case guardobj:
            if(dist <= 12 && (ret->state == &s_grdshoot2 || ret->state == &s_grdshoot3))
                return true;
            break;
        case officerobj:
            if(dist <= 14 && (ret->state == &s_ofcshoot2 || ret->state == &s_ofcshoot3))
                return true;
            break;
        case ssobj:
            if(dist <= 16 && (ret->state == &s_ssshoot2 || ret->state == &s_ssshoot3 || ret->state == &s_ssshoot4
                              || ret->state == &s_ssshoot5 || ret->state == &s_ssshoot6 || ret->state == &s_ssshoot7
                              || ret->state == &s_ssshoot8 || ret->state == &s_ssshoot9))
                return true;
            break;
        case mutantobj:
            if(dist <= 2 && ret->flags & FL_ATTACKMODE
               || dist <= 14 && (ret->state == &s_mutshoot1 || ret->state == &s_mutshoot2 || ret->state == &s_mutshoot3 ||
                                 ret->state == &s_mutshoot4))
                return true;
            break;
#ifndef SPEAR
        case bossobj:
            if(dist <= 16 && (ret->state == &s_bossshoot1 || ret->state == &s_bossshoot2 || ret->state == &s_bossshoot3 || ret->state == &s_bossshoot4
                              || ret->state == &s_bossshoot5 || ret->state == &s_bossshoot6 || ret->state == &s_bossshoot7
                              || ret->state == &s_bossshoot8))
                return true;
            break;
        case ghostobj:
            if(dist <= 2)
                return true;
            break;
        case mechahitlerobj:
            if(dist <= 16 && (ret->state == &s_mechashoot1 || ret->state == &s_mechashoot2 || ret->state == &s_mechashoot3 || ret->state == &s_mechashoot4
                              || ret->state == &s_mechashoot5 || ret->state == &s_mechashoot6))
                return true;
            break;
        case realhitlerobj:
            if(dist <= 16 && (ret->state == &s_hitlershoot1 || ret->state == &s_hitlershoot2 || ret->state == &s_hitlershoot3 || ret->state == &s_hitlershoot4
                              || ret->state == &s_hitlershoot5 || ret->state == &s_hitlershoot6))
                return true;
            break;
        case gretelobj:
            if(dist <= 16 && (ret->state == &s_gretelshoot1 || ret->state == &s_gretelshoot2 || ret->state == &s_gretelshoot3
                              || ret->state == &s_gretelshoot4
                              || ret->state == &s_gretelshoot5 || ret->state == &s_gretelshoot6 || ret->state == &s_gretelshoot7
                              || ret->state == &s_gretelshoot8))
                return true;
            break;
        case fatobj:
            if(dist <= 16 && (ret->state == &s_fatshoot1 || ret->state == &s_fatshoot2 || ret->state == &s_fatshoot3 || ret->state == &s_fatshoot4
                              || ret->state == &s_fatshoot5 || ret->state == &s_fatshoot6))
                return true;
            break;
#else
        case transobj:
            if(dist <= 16 && (ret->state == &s_transshoot1 || ret->state == &s_transshoot2 || ret->state == &s_transshoot3
                              || ret->state == &s_transshoot4
                              || ret->state == &s_transshoot5 || ret->state == &s_transshoot6 || ret->state == &s_transshoot7
                              || ret->state == &s_transshoot8))
                return true;
            break;
        case willobj:
            if(dist <= 16 && (ret->state == &s_willshoot1 || ret->state == &s_willshoot2 || ret->state == &s_willshoot3 || ret->state == &s_willshoot4
                              || ret->state == &s_willshoot5 || ret->state == &s_willshoot6))
                return true;
            break;
        case uberobj:
            if(dist <= 16 && (ret->state == &s_ubershoot1 || ret->state == &s_ubershoot2 || ret->state == &s_ubershoot3 || ret->state == &s_ubershoot4
                              || ret->state == &s_ubershoot5 || ret->state == &s_ubershoot6 || ret->state == &s_ubershoot7))
                return true;
            break;
        case deathobj:
            if(dist <= 16 && (ret->state == &s_deathshoot1 || ret->state == &s_deathshoot2 || ret->state == &s_deathshoot3 || ret->state == &s_deathshoot4
                              || ret->state == &s_deathshoot5))
                return true;
            break;
#endif
	}
	return false;
}

//
// Basic::GenericCheckLine
//
// Like CheckLine, but with user-settable coordinates
//
boolean Basic_GenericCheckLine (long x1, long y1, int x2, int y2)
{
    int         xt1,yt1,xt2,yt2;
    int         x,y;
    int         xdist,ydist,xstep,ystep;
    int         partial,delta;
    long     ltemp;
    int         xfrac,yfrac,deltafrac;
    unsigned    value,intercept;
    
    x1 >>= UNSIGNEDSHIFT;            // 1/256 tile precision
    y1 >>= UNSIGNEDSHIFT;
    xt1 = x1 >> 8;
    yt1 = y1 >> 8;
    
    x2 >>= UNSIGNEDSHIFT;
    y2 >>= UNSIGNEDSHIFT;
    xt2 = x2 >> 8;
    yt2 = y2 >> 8;
    
    xdist = abs(xt2-xt1);
    
    if (xdist > 0)
    {
        if (xt2 > xt1)
        {
            partial = 256-(x1&0xff);
            xstep = 1;
        }
        else
        {
            partial = x1&0xff;
            xstep = -1;
        }
        
        deltafrac = abs(x2-x1);
        delta = y2-y1;
		  ltemp = ((long)delta<<8)/deltafrac;
		  if (ltemp > 0x7fffl)
				ystep = 0x7fff;
		  else if (ltemp < -0x7fffl)
				ystep = -0x7fff;
		  else
				ystep = ltemp;
		  yfrac = y1 + (((long)ystep*partial) >>8);

		  x = xt1+xstep;
		  xt2 += xstep;
		  do
		  {
				y = yfrac>>8;
				yfrac += ystep;

				value = (unsigned)tilemap[x][y];
				x += xstep;

				if (!value)
					 continue;

				if (value<128 || value>256)
					 return false;

				//
				// see if the door is open enough
				//
				value &= ~0x80;
				intercept = yfrac-ystep/2;

				if (intercept>doorposition[value])
					 return false;

		  } while (x != xt2);
	 }

	 ydist = abs(yt2-yt1);

	 if (ydist > 0)
	 {
		  if (yt2 > yt1)
		  {
				partial = 256-(y1&0xff);
				ystep = 1;
		  }
		  else
		  {
				partial = y1&0xff;
				ystep = -1;
		  }

		  deltafrac = abs(y2-y1);
		  delta = x2-x1;
		  ltemp = ((long)delta<<8)/deltafrac;
		  if (ltemp > 0x7fffl)
				xstep = 0x7fff;
		  else if (ltemp < -0x7fffl)
				xstep = -0x7fff;
		  else
				xstep = ltemp;
		  xfrac = x1 + (((long)xstep*partial) >>8);

		  y = yt1 + ystep;
		  yt2 += ystep;
		  do
		  {
				x = xfrac>>8;
				xfrac += xstep;

				value = (unsigned)tilemap[x][y];
				y += ystep;

				if (!value)
					 continue;

				if (value<128 || value>256)
					 return false;

				//
				// see if the door is open enough
				//
				value &= ~0x80;
				intercept = xfrac-xstep/2;

				if (intercept>doorposition[value])
					 return false;
		  } while (y != yt2);
	 }

	 return true;
}

boolean Basic_IsBoss(classtype cls)
{
    switch (cls)
    {
        case bossobj:
        case schabbobj:
        case fakeobj:
        case mechahitlerobj:
        case realhitlerobj:
        case gretelobj:
        case giftobj:
        case fatobj:
        case angelobj:
        case transobj:
        case uberobj:
        case willobj:
        case deathobj:
            return true;


	 }
    return false;
}

boolean Basic_IsEnemy(classtype cls)
{
	switch(cls)
	{
        case guardobj:
        case officerobj:
        case ssobj:
        case dogobj:
        case bossobj:
        case schabbobj:
        case fakeobj:
        case mechahitlerobj:
        case mutantobj:
        case ghostobj:
        case realhitlerobj:
        case gretelobj:
        case giftobj:
        case fatobj:
        case spectreobj:
        case angelobj:
        case transobj:
        case uberobj:
        case willobj:
        case deathobj:
            return true;
	}
	return false;
}

