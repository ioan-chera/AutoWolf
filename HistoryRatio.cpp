/*
 *  HistoryRatio.cpp
 *  AutoWolf
 *
 *  Created by MacBook on 28.06.2012.
 *  Copyright 2012 Ioan Chera. Written under the GNU GPL license.
 *
 */

#include "HistoryRatio.h"

void HistoryRatio::addFail()
{
	value -= pipe & 1;
	pipe >>= 1;
}

void HistoryRatio::addSuccess()
{
	value += !(pipe & 1);
	pipe = (pipe >> 1) + (1 << (length - 1));
}

void HistoryRatio::initLength(int len)
{
	value = length = len;
	pipe = (1 << len) - 1;
}