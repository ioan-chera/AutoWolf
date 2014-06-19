// Copyright (C) 2014  Ioan Chera
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

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "Config.h"
#include "cvstuff.h"
#include "StdStringExtensions.h"

static cv::VideoWriter g_writer;
static std::string g_moviepath;
static cv::Mat g_frame;

bool InitSessionVideo()
{
    char name[80];
    snprintf(name, 80, "%llu", (unsigned long long)time(NULL));
    g_moviepath = cfg_dir;
    ConcatSubpath(g_moviepath, name);
    g_moviepath += ".avi";
    
    g_writer.open(g_moviepath, CV_FOURCC('P', 'I', 'M', '1'), 70, cv::Size(cfg_screenWidth, cfg_screenHeight), false);
    if(!g_writer.isOpened())
        return false;
    
    g_frame = cv::Mat(cfg_screenHeight, cfg_screenWidth, CV_8UC1);
    
    return true;
}

byte* GetMat()
{
    return g_frame.data;
}

void SaveSessionVideo()
{
    
}

void WriteMat()
{
    g_writer.write(g_frame);
}