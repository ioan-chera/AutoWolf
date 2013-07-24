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

#include "wl_def.h"
#include "wl_draw.h"
#include "wl_game.h"
#include "wl_play.h"
#include "CheckSum.h"
#include "Config.h"
#include "cv_video.h"

AutoWolfVideoEngine videoEngine;

inline static byte acquirePlaneColour(byte index)
{
   if(index == IMPALED(vgaCeiling, [gamestate.episode * 10 + mapon]))
      return 0x40;
   if(index == FLOOR_COLOUR) // floor
      return 0x80;
   return 0;
}

AutoWolfVideoEngine::AutoWolfVideoEngine(int inFourcc, double inFps,
                                         cv::Size inSize)
{
   fourcc = inFourcc;
   fps = inFps;
   size = inSize;
}

void AutoWolfVideoEngine::startRecord()
{
   PString aviFilePath(cfg_dir);
//   PString aviFileHash;
//   PString temp(gamestate.mapon);
//   mapsegsChecksum.GetHumanReadableHex(aviFileHash);
   aviFilePath.concatSubpath(PString(gamestate.episode).concat(gamestate.mapon).
                             concatExtension("avi"));
   writer.open(aviFilePath(), fourcc, fps, size, false);
}

void AutoWolfVideoEngine::processSDLSurface(const SDL_Surface *surf)
{
   if(!writer.isOpened())
      return;
   cv::Mat frame;
   frame.create(size, CV_8UC1);
   
   int xstep = surf->w / size.width, ystep = surf->h / size.height;
   int x, y;
   for(y = 0; y < size.height; ++y)
   {
      for(x = 0; x < size.width; ++x)
      {
//         int test =
         frame.at<byte>(y, x) =
         acquirePlaneColour(((byte *)surf->pixels)[xstep * x +
                                                   surf->w * ystep * y]);
      }
   }
   cv::medianBlur(frame, frame, 5);
   for(int i = 0; i < tics; ++i)
      writer << frame;
}