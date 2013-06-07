#ifndef IOAN_BAS_H
#define IOAN_BAS_H

//
// IsBoss
//
inline static boolean Basic_IsBoss(classtype cls)
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
            break;
            
        default:
            return false;
    }
    return false;
}

//
// IsEnemy
//
inline static boolean Basic_IsEnemy(classtype cls)
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

//
// Major
//
inline static int Basic_Major(int t)
{
    return (t << TILESHIFT) + (1 << (TILESHIFT - 1));
}

// Check if it's armed
boolean Basic_IsDamaging(objtype *ret, int dist);

// Generic check line
 boolean Basic_GenericCheckLine (long x1, long y1, int x2, int y2);


#endif
