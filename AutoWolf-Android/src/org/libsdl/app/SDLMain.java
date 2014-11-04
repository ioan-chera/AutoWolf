package org.libsdl.app;

/**
Simple nativeInit() runnable
*/
class SDLMain implements Runnable 
{
	@Override
	public void run() 
	{
	    // Runs SDL_main()
		String[] arr = SDLActivity.sArgs.toArray(new String[SDLActivity.sArgs.size()]);
	    SDLActivity.nativeInit(arr);
	
	    //Log.v("SDL", "SDL thread terminated");
	}
}
