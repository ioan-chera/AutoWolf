/*
    SDL_android_main.c, placed in the public domain by Sam Lantinga  3/13/14
*/
#include "../../SDL_internal.h"

#ifdef __ANDROID__

/* Include the SDL main definition header */
#include "SDL_main.h"

/*******************************************************************************
                 Functions called by JNI
*******************************************************************************/
#include <jni.h>

/* Called before SDL_main() to initialize JNI bindings in SDL library */
extern void SDL_Android_Init(JNIEnv* env, jclass cls);

/* Start up the SDL app */
void Java_org_libsdl_app_SDLActivity_nativeInit(JNIEnv* env, jclass cls, jobjectArray jArgs)
{
    /* This interface could expand with ABI negotiation, calbacks, etc. */
    SDL_Android_Init(env, cls);

    SDL_SetMainReady();

    int stringCount = (*env)->GetArrayLength(env, (jarray)jArgs);
    int i;

    char** argv = (char**)calloc(stringCount + 2, sizeof(char*));
    argv[0] = SDL_strdup("SDL_app");

	for (i=0; i < stringCount; i++)
	{
		jstring string = (jstring) (*env)->GetObjectArrayElement(env, jArgs, i);
		argv[i + 1] = (char*)(*env)->GetStringUTFChars(env, string, 0);
		// Don't forget to call `ReleaseStringUTFChars` when you're done.
	}
	argv[i + 1] = NULL;

    /* Run the application code! */
    int status;
    status = SDL_main(stringCount + 1, argv);

    /* Do not issue an exit or the whole application will terminate instead of just the SDL thread */
    /* exit(status); */
}

#endif /* __ANDROID__ */

/* vi: set ts=4 sw=4 expandtab: */
