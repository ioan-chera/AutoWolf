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
////////////////////////////////////////////////////////////////////////////////
//
// OpenCV control class
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __Wolf4SDL__cv_video__
#define __Wolf4SDL__cv_video__

class AutoWolfVideoEngine
{
   cv::VideoWriter writer;
   int fourcc;
   double fps;
   cv::Size size;
public:
   AutoWolfVideoEngine(int inFourcc = CV_FOURCC('M', 'J', 'P', 'G'),
                       double inFps = 70, cv::Size inSize = cv::Size(320, 200));
   void startRecord();
   
   void processSDLSurface(const SDL_Surface *surf) ;
};

extern AutoWolfVideoEngine videoEngine;

#endif /* defined(__Wolf4SDL__cv_video__) */
