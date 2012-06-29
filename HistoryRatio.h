/*
 *  HistoryRatio.h
 *  AutoWolf
 *
 *  Created by MacBook on 28.06.2012.
 *  Copyright 2012 Ioan Chera. Written under the GNU GPL license.
 *
 */

#ifndef HISTORYRATIO_H_
#define HISTORYRATIO_H_

//
// HistoryRatio
//
// Used to count past ratios
//
class HistoryRatio
{
private:
	int pipe, value, length;
public:
	void addFail();
	void addSuccess();
	void initLength(int len);
	
	int getValue()
	{
		return value;
	}
};

#endif