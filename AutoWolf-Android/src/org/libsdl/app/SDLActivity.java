package org.libsdl.app;

import java.util.ArrayList;
import java.util.Arrays;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.hardware.Sensor;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Display;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;
import android.widget.AbsoluteLayout;


/**
    SDL Activity
*/
public class SDLActivity extends Activity 
{
    private static final String TAG = "SDL";

    // Keep track of the paused state
    public static boolean sIsPaused, sIsSurfaceReady, sHasFocus;
    public static boolean sExitCalledFromJava;

    // Main components
    protected static SDLActivity sSingleton;
    protected static SDLSurface sSurface;
    protected static View sTextEdit;
    protected static ViewGroup sLayout;
    protected static SDLJoystickHandler sJoystickHandler;

    // This is what SDL runs in. It invokes SDL_main(), eventually
    protected static Thread sSDLThread;
    
    // Audio
    protected static AudioTrack sAudioTrack;
    
    // ARGS
    public static final String EXTRA_ARGS = "args";
    static ArrayList<String> sArgs;
    
    public static boolean sDemandNorestore;

    // Load the .so
    static 
    {
        System.loadLibrary("SDL2");
        //System.loadLibrary("SDL2_image");
        System.loadLibrary("SDL2_mixer");
        //System.loadLibrary("SDL2_net");
        //System.loadLibrary("SDL2_ttf");
        System.loadLibrary("main");
    }
    
    
    public static void initialize() 
    {
        // The static nature of the singleton and Android quirkyness force us to
    	// initialize everything here
        // Otherwise, when exiting the app and returning to it, these variables 
    	// *keep* their pre exit values
        sSingleton = null;
        sSurface = null;
        sTextEdit = null;
        sLayout = null;
        sJoystickHandler = null;
        sSDLThread = null;
        sAudioTrack = null;
        sExitCalledFromJava = false;
        sIsPaused = false;
        sIsSurfaceReady = false;
        sHasFocus = true;
    }

    // Setup
    @Override
    protected void onCreate(Bundle savedInstanceState) 
    {
        Log.v("SDL", "onCreate():" + sSingleton);
        super.onCreate(savedInstanceState);
        
        Intent intent = getIntent();
        sArgs = intent.getStringArrayListExtra(EXTRA_ARGS);
        if(sDemandNorestore)
        {
        	sDemandNorestore = false;
        	sArgs.add("--norestore");
        }
        
        SDLActivity.initialize();
        // So we can call stuff from static callbacks
        sSingleton = this;

        // Set up the surface
        sSurface = new SDLSurface(getApplication());
        
        if(Build.VERSION.SDK_INT >= 12) {
            sJoystickHandler = new SDLJoystickHandler_API12();
        }
        else 
        {
            sJoystickHandler = new SDLJoystickHandler();
        }

        sLayout = new AbsoluteLayout(this);
//        sLayout.setBackgroundColor(Color.BLACK);
        sLayout.addView(sSurface);
//        sSurface.getLayoutParams().width = 400;

        setContentView(sLayout);
        
        updateSurfaceSize();
        
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }
    
    public static void updateSurfaceSize()
    {
    	final int requestedWidth = 640, requestedHeight = 480;
    	// do the math
    	final int x, y, width, height;
    	
    	Display display = sSingleton.getWindowManager().getDefaultDisplay();
    	
    	int screenWidth = Math.max(display.getWidth(), display.getHeight());
    	int screenHeight = Math.min(display.getWidth(), display.getHeight());;
    	Log.d("SDLActivity", "Screen size: " + screenWidth + "*" + screenHeight);
    	
    	if(screenWidth <= 0 || screenHeight <= 0)
    	{
    		Log.e("SDLActivity", "Bad screen size!");
    		return;
    	}
    	
    	double ratio = (double)requestedWidth / requestedHeight;
    	double screenRatio = (double)screenWidth / screenHeight;
    	
    	if(ratio > screenRatio)
    	{
    		// request wider than screen
    		width = screenWidth;
    		height = (int)(width / ratio);
    		x = 0;
    		y = (screenHeight - height) / 2;
    	}
    	else
    	{
    		height = screenHeight;
    		width = (int)(height * ratio);
    		y = 0;
    		x = (screenWidth - width) / 2;
    	}
    	
    	ViewGroup.LayoutParams vglp = sSurface.getLayoutParams();
    	AbsoluteLayout.LayoutParams allp;

		if(vglp instanceof AbsoluteLayout.LayoutParams)
    	{
    		allp = (AbsoluteLayout.LayoutParams)vglp;
    		Log.d("SDLActivity", "allp is vglp");
    	}
	
    	else
    	{
    		allp = new AbsoluteLayout.LayoutParams(width, height, x, y);
    		sSurface.setLayoutParams(allp);
    		Log.d("SDLActivity", "allp replaces vglp");
    	}
		allp.width = width;
		allp.height = height;
		allp.x = x;
		allp.y = y;
		
		Log.d("SDLActivity", "Updated screen size to " + width + " " + height + " " + x + " " + y);
    }
    
    // Events
    @Override
    protected void onPause() 
    {
        Log.v("SDL", "onPause()");
        super.onPause();
        SDLActivity.handlePause();
    }

    @Override
    protected void onResume() 
    {
        Log.v("SDL", "onResume()");
        super.onResume();
        SDLActivity.handleResume();
    }


    @Override
    public void onWindowFocusChanged(boolean hasFocus) 
    {
        super.onWindowFocusChanged(hasFocus);
        Log.v("SDL", "onWindowFocusChanged(): " + hasFocus);

        SDLActivity.sHasFocus = hasFocus;
        if (hasFocus) 
        {
            SDLActivity.handleResume();
        }
    }

    @Override
    public void onLowMemory() 
    {
        Log.v("SDL", "onLowMemory()");
        super.onLowMemory();
        SDLActivity.nativeLowMemory();
    }

    @Override
    protected void onDestroy() 
    {
        Log.v("SDL", "onDestroy()");
        // Send a quit message to the application
        SDLActivity.sExitCalledFromJava = true;
        SDLActivity.nativeQuit();

        // Now wait for the SDL thread to quit
        if (SDLActivity.sSDLThread != null) 
        {
            try 
            {
                SDLActivity.sSDLThread.join();
            } 
            catch(Exception e) 
            {
                Log.v("SDL", "Problem stopping thread: " + e);
            }
            SDLActivity.sSDLThread = null;

            //Log.v("SDL", "Finished waiting for SDL thread");
        }
            
        super.onDestroy();
        // Reset everything in case the user re opens the app
        SDLActivity.initialize();
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) 
    {
        int keyCode = event.getKeyCode();
        // Ignore certain special keys so they're handled by Android
        if (keyCode == KeyEvent.KEYCODE_VOLUME_DOWN ||
            keyCode == KeyEvent.KEYCODE_VOLUME_UP ||
            keyCode == KeyEvent.KEYCODE_CAMERA ||
            keyCode == 168 || /* API 11: KeyEvent.KEYCODE_ZOOM_IN */
            keyCode == 169 /* API 11: KeyEvent.KEYCODE_ZOOM_OUT */
            ) 
        {
            return false;
        }
        return super.dispatchKeyEvent(event);
    }

    /** Called by onPause or surfaceDestroyed. Even if surfaceDestroyed
     *  is the first to be called, mIsSurfaceReady should still be set
     *  to 'true' during the call to onPause (in a usual scenario).
     */
    public static void handlePause() 
    {
        if(!SDLActivity.sIsPaused && SDLActivity.sIsSurfaceReady) 
        {
            SDLActivity.sIsPaused = true;
            SDLActivity.nativePause();
            sSurface.enableSensor(Sensor.TYPE_ACCELEROMETER, false);
        }
    }

    /** Called by onResume or surfaceCreated. An actual resume should be done only when the surface is ready.
     * Note: Some Android variants may send multiple surfaceChanged events, so we don't need to resume
     * every time we get one of those events, only if it comes after surfaceDestroyed
     */
    public static void handleResume() 
    {
        if (SDLActivity.sIsPaused && SDLActivity.sIsSurfaceReady && SDLActivity.sHasFocus) 
        {
            SDLActivity.sIsPaused = false;
            SDLActivity.nativeResume();
            sSurface.enableSensor(Sensor.TYPE_ACCELEROMETER, true);
        }
    }
        
    /* The native thread has finished */
    public static void handleNativeExit() 
    {
        SDLActivity.sSDLThread = null;
        sSingleton.finish();
    }


    // Messages from the SDLMain thread
    static final int COMMAND_CHANGE_TITLE = 1;
    static final int COMMAND_UNUSED = 2;
    static final int COMMAND_TEXTEDIT_HIDE = 3;

    protected static final int COMMAND_USER = 0x8000;

    /**
     * This method is called by SDL if SDL did not handle a message itself.
     * This happens if a received message contains an unsupported command.
     * Method can be overwritten to handle Messages in a different class.
     * @param command the command of the message.
     * @param param the parameter of the message. May be null.
     * @return if the message was handled in overridden method.
     */
    protected boolean onUnhandledMessage(int command, Object param) 
    {
        return false;
    }

    /**
     * A Handler class for Messages from native SDL applications.
     * It uses current Activities as target (e.g. for the title).
     * static to prevent implicit references to enclosing object.
     */
    protected static class SDLCommandHandler extends Handler 
    {
        @Override
        public void handleMessage(Message msg) 
        {
            Context context = getContext();
            if (context == null) 
            {
                Log.e(TAG, "error handling message, getContext() returned null");
                return;
            }
            switch (msg.arg1) 
            {
            case COMMAND_CHANGE_TITLE:
                if (context instanceof Activity) {
                    ((Activity) context).setTitle((String)msg.obj);
                } else {
                    Log.e(TAG, "error handling message, getContext() returned no Activity");
                }
                break;
            case COMMAND_TEXTEDIT_HIDE:
                if (sTextEdit != null) {
                    sTextEdit.setVisibility(View.GONE);

                    InputMethodManager imm = (InputMethodManager) context.getSystemService(Context.INPUT_METHOD_SERVICE);
                    imm.hideSoftInputFromWindow(sTextEdit.getWindowToken(), 0);
                }
                break;

            default:
                if ((context instanceof SDLActivity) && !((SDLActivity) context).onUnhandledMessage(msg.arg1, msg.obj)) {
                    Log.e(TAG, "error handling message, command is " + msg.arg1);
                }
            }
        }
    }

    // Handler for the messages
    Handler commandHandler = new SDLCommandHandler();

    // Send a message from the SDLMain thread
    boolean sendCommand(int command, Object data) {
        Message msg = commandHandler.obtainMessage();
        msg.arg1 = command;
        msg.obj = data;
        return commandHandler.sendMessage(msg);
    }

    // C functions we call
    public static native void nativeInit(String[] args);
    public static native void nativeLowMemory();
    public static native void nativeQuit();
    public static native void nativePause();
    public static native void nativeResume();
    public static native void onNativeResize(int x, int y, int format);
    public static native int onNativePadDown(int device_id, int keycode);
    public static native int onNativePadUp(int device_id, int keycode);
    public static native void onNativeJoy(int device_id, int axis,
                                          float value);
    public static native void onNativeHat(int device_id, int hat_id,
                                          int x, int y);
    public static native void onNativeKeyDown(int keycode);
    public static native void onNativeKeyUp(int keycode);
    public static native void onNativeKeyboardFocusLost();
    public static native void onNativeTouch(int touchDevId, int pointerFingerId,
                                            int action, float x, 
                                            float y, float p);
    public static native void onNativeAccel(float x, float y, float z);
    public static native void onNativeSurfaceChanged();
    public static native void onNativeSurfaceDestroyed();
    public static native void nativeFlipBuffers();
    public static native int nativeAddJoystick(int device_id, String name, 
                                               int is_accelerometer, int nbuttons, 
                                               int naxes, int nhats, int nballs);
    public static native int nativeRemoveJoystick(int device_id);

    public static void flipBuffers() {
        SDLActivity.nativeFlipBuffers();
    }

    public static boolean setActivityTitle(String title) {
        // Called from SDLMain() thread and can't directly affect the view
        return sSingleton.sendCommand(COMMAND_CHANGE_TITLE, title);
    }

    public static boolean sendMessage(int command, int param) {
        return sSingleton.sendCommand(command, Integer.valueOf(param));
    }

    public static Context getContext() {
        return sSingleton;
    }

    /**
     * @return result of getSystemService(name) but executed on UI thread.
     */
    public Object getSystemServiceFromUiThread(final String name) {
        final Object lock = new Object();
        final Object[] results = new Object[2]; // array for writable variables
        synchronized (lock) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    synchronized (lock) {
                        results[0] = getSystemService(name);
                        results[1] = Boolean.TRUE;
                        lock.notify();
                    }
                }
            });
            if (results[1] == null) {
                try {
                    lock.wait();
                } catch (InterruptedException ex) {
                    ex.printStackTrace();
                }
            }
        }
        return results[0];
    }

    static class ShowTextInputTask implements Runnable {
        /*
         * This is used to regulate the pan&scan method to have some offset from
         * the bottom edge of the input region and the top edge of an input
         * method (soft keyboard)
         */
        static final int HEIGHT_PADDING = 15;

        public int x, y, w, h;

        public ShowTextInputTask(int x, int y, int w, int h) {
            this.x = x;
            this.y = y;
            this.w = w;
            this.h = h;
        }

        @Override
        public void run() {
            AbsoluteLayout.LayoutParams params = new AbsoluteLayout.LayoutParams(
                    w, h + HEIGHT_PADDING, x, y);

            if (sTextEdit == null) {
                sTextEdit = new DummyEdit(getContext());

                sLayout.addView(sTextEdit, params);
            } else {
                sTextEdit.setLayoutParams(params);
            }

            sTextEdit.setVisibility(View.VISIBLE);
            sTextEdit.requestFocus();

            InputMethodManager imm = (InputMethodManager) getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
            imm.showSoftInput(sTextEdit, 0);
        }
    }

    public static boolean showTextInput(int x, int y, int w, int h) {
        // Transfer the task to the main thread as a Runnable
        return sSingleton.commandHandler.post(new ShowTextInputTask(x, y, w, h));
    }
            
    public static Surface getNativeSurface() {
        return SDLActivity.sSurface.getNativeSurface();
    }

    // Audio
    public static int audioInit(int sampleRate, boolean is16Bit, boolean isStereo, int desiredFrames) {
        int channelConfig = isStereo ? AudioFormat.CHANNEL_CONFIGURATION_STEREO : AudioFormat.CHANNEL_CONFIGURATION_MONO;
        int audioFormat = is16Bit ? AudioFormat.ENCODING_PCM_16BIT : AudioFormat.ENCODING_PCM_8BIT;
        int frameSize = (isStereo ? 2 : 1) * (is16Bit ? 2 : 1);
        
        Log.v("SDL", "SDL audio: wanted " + (isStereo ? "stereo" : "mono") + " " + (is16Bit ? "16-bit" : "8-bit") + " " + (sampleRate / 1000f) + "kHz, " + desiredFrames + " frames buffer");
        
        // Let the user pick a larger buffer if they really want -- but ye
        // gods they probably shouldn't, the minimums are horrifyingly high
        // latency already
        desiredFrames = Math.max(desiredFrames, (AudioTrack.getMinBufferSize(sampleRate, channelConfig, audioFormat) + frameSize - 1) / frameSize);
        
        if (sAudioTrack == null) {
            sAudioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, sampleRate,
                    channelConfig, audioFormat, desiredFrames * frameSize, AudioTrack.MODE_STREAM);
            
            // Instantiating AudioTrack can "succeed" without an exception and the track may still be invalid
            // Ref: https://android.googlesource.com/platform/frameworks/base/+/refs/heads/master/media/java/android/media/AudioTrack.java
            // Ref: http://developer.android.com/reference/android/media/AudioTrack.html#getState()
            
            if (sAudioTrack.getState() != AudioTrack.STATE_INITIALIZED) {
                Log.e("SDL", "Failed during initialization of Audio Track");
                sAudioTrack = null;
                return -1;
            }
            
            sAudioTrack.play();
        }
       
        Log.v("SDL", "SDL audio: got " + ((sAudioTrack.getChannelCount() >= 2) ? "stereo" : "mono") + " " + ((sAudioTrack.getAudioFormat() == AudioFormat.ENCODING_PCM_16BIT) ? "16-bit" : "8-bit") + " " + (sAudioTrack.getSampleRate() / 1000f) + "kHz, " + desiredFrames + " frames buffer");
        
        return 0;
    }
    
    public static void audioWriteShortBuffer(short[] buffer) {
        for (int i = 0; i < buffer.length; ) {
            int result = sAudioTrack.write(buffer, i, buffer.length - i);
            if (result > 0) {
                i += result;
            } else if (result == 0) {
                try {
                    Thread.sleep(1);
                } catch(InterruptedException e) {
                    // Nom nom
                }
            } else {
                Log.w("SDL", "SDL audio: error return from write(short)");
                return;
            }
        }
    }
    
    public static void audioWriteByteBuffer(byte[] buffer) {
        for (int i = 0; i < buffer.length; ) {
            int result = sAudioTrack.write(buffer, i, buffer.length - i);
            if (result > 0) {
                i += result;
            } else if (result == 0) {
                try {
                    Thread.sleep(1);
                } catch(InterruptedException e) {
                    // Nom nom
                }
            } else {
                Log.w("SDL", "SDL audio: error return from write(byte)");
                return;
            }
        }
    }

    public static void audioQuit() {
        if (sAudioTrack != null) {
            sAudioTrack.stop();
            sAudioTrack = null;
        }
    }

    // Input

    /**
     * @return an array which may be empty but is never null.
     */
    public static int[] inputGetInputDeviceIds(int sources) {
        int[] ids = InputDevice.getDeviceIds();
        int[] filtered = new int[ids.length];
        int used = 0;
        for (int i = 0; i < ids.length; ++i) {
            InputDevice device = InputDevice.getDevice(ids[i]);
            if ((device != null) && ((device.getSources() & sources) != 0)) {
                filtered[used++] = device.getId();
            }
        }
        return Arrays.copyOf(filtered, used);
    }
            
    // Joystick glue code, just a series of stubs that redirect to the SDLJoystickHandler instance
    public static boolean handleJoystickMotionEvent(MotionEvent event) {
        return sJoystickHandler.handleMotionEvent(event);
    }
    
    public static void pollInputDevices() {
        if (SDLActivity.sSDLThread != null) {
            sJoystickHandler.pollInputDevices();
        }
    }
    
}
