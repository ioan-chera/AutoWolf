//
// IOAN_SECRET.H
//
// File by Ioan Chera, started 25.10.2012
//
// Functions handling the secret maze solving logic
//

#ifndef IOAN_SECRET_H_
#define IOAN_SECRET_H_

class Secret
{
protected:
	static void CalcScoreAdd(int tx, int ty);	// CalcScore recursive component
public:
	static int CalcScore(int tx, int ty);	// calculate the available score from this current position
};

#endif
