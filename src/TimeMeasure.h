//
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

#ifndef TIMEMEASURE_H_
#define TIMEMEASURE_H_

#include <SDL.h>

#define SET_TIME_MEASURE(a) TimeMeasure<a> TIME_MEASURE(__FUNCTION__)

template<Uint32 threshold = 10> class TimeMeasure
{
	Uint32 clk;
	const char* tag;
public:
	TimeMeasure(const char* intag)
	{
		tag = intag;
		clk = SDL_GetTicks();
	}

	~TimeMeasure()
	{
		clk = SDL_GetTicks() - clk;
		if (clk >= threshold)
			Logger::Write("%s: %u", tag, static_cast<unsigned>(clk));
	}
};

#endif