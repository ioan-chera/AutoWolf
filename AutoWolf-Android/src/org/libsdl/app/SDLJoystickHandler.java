package org.libsdl.app;

import android.view.MotionEvent;

/* A null joystick handler for API level < 12 devices (the accelerometer is handled separately) */
class SDLJoystickHandler {
    
    public boolean handleMotionEvent(MotionEvent event) {
        return false;
    }
    
    public void pollInputDevices() {
    }
}
