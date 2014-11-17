//
//  Platform.cpp
//  AutoWolf-iOS
//
//  Created by ioan on 16.11.2014.
//  Copyright (c) 2014 Ioan Chera. All rights reserved.
//

#include "version.h"
#include "Platform.h"

bool Platform::touchscreen;

void Platform::DetermineConfig()
{
#if defined(IOS) || defined(__ANDROID__)
    touchscreen = true;
#endif
}