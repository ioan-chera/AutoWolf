LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../AutoWolf-Android/jni/SDL
SDLMIXER_PATH := ../AutoWolf-Android/jni/SDL_mixer

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include

LOCAL_CPP_FEATURES += exceptions
LOCAL_CPPFLAGS += -std=gnu++11

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
	ActorStates.cpp \
	CheckSum.cpp \
	CommandLine.cpp \
	Config.cpp \
	DataFile.cpp \
	DirectoryFile.cpp \
	e_hashkeys.cpp \
	Exception.cpp \
	FileSystem.cpp \
	HistoryRatio.cpp \
	i_system.cpp \
	i_video.cpp \
	id_ca.cpp \
	id_in.cpp \
	id_pm.cpp \
	id_sd.cpp \
	id_us_1.cpp \
	id_vh.cpp \
	id_vl.cpp \
	ioan_bas.cpp \
	ioan_bot.cpp \
	ioan_secret.cpp \
	Logger.cpp \
	m_buffer.cpp \
	MapExploration.cpp \
	MasterDirectoryFile.cpp \
	obattrib.cpp \
	PathArray.cpp \
  Platform.cpp \
	Property.cpp \
	PropertyFile.cpp \
	PString.cpp \
	ShellUnicode.cpp \
	signon.cpp \
	SODFlag.cpp \
	StdStringExtensions.cpp \
	wl_act1.cpp \
	wl_act2.cpp \
	wl_agent.cpp \
	wl_atmos.cpp \
	wl_cloudsky.cpp \
	wl_debug.cpp \
	wl_dir3dspr.cpp \
	wl_draw.cpp \
	wl_floorceiling.cpp \
	wl_game.cpp \
	wl_inter.cpp \
	wl_main.cpp \
	wl_menu.cpp \
	wl_parallax.cpp \
	wl_play.cpp \
	wl_shade.cpp \
	wl_state.cpp \
	wl_text.cpp \
	dosbox/dbopl.cpp

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_mixer

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
