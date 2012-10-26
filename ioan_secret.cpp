//
// IOAN_SECRET.H
//
// File by Ioan Chera, started 25.10.2012
//
// Functions handling the secret maze solving logic
//
#include "ioan_secret.h"

//
// Secret::CalcScoreAdd
//
// Recursive function for CalcScore
//
void Secret::CalcScoreAdd(int tx, int ty)
{
	static int totalscore = 0;
	
	// list of objects at [tx][ty]
	
}

//
// Secret::CalcScore
//
// calculate the available score from this current position
//
int Secret::CalcScore(int tx, int ty)
{
	int ret = 0;

	CalcScoreAdd(tx, ty);
	
	return ret;
}

//

