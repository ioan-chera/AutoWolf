package org.libsdl.app;

import android.annotation.SuppressLint;
import android.view.MotionEvent;
import android.view.View;

@SuppressLint("NewApi")
class SDLGenericMotionListener_API12 implements View.OnGenericMotionListener {
    // Generic Motion (mouse hover, joystick...) events go here
    // We only have joysticks yet
    @Override
    public boolean onGenericMotion(View v, MotionEvent event) {
        return SDLActivity.handleJoystickMotionEvent(event);
    }
}
