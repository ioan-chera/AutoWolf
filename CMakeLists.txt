cmake_minimum_required(VERSION 3.14)
project(AutoWolf VERSION 1.0.0 LANGUAGES CXX C)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(AUTOWOLF_DEBUG_MAX "Enable maximum debug features" OFF)

# Include FetchContent for downloading dependencies
include(FetchContent)

# =============================================================================
# EXTERNAL DEPENDENCIES
# =============================================================================

# Fetch SDL2
FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        release-2.28.5  # or whatever version you prefer
    GIT_SHALLOW    TRUE
)

# Fetch SDL2_mixer
FetchContent_Declare(
    SDL2_mixer
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
    GIT_TAG        release-2.6.3   # or whatever version you prefer
    GIT_SHALLOW    TRUE
)

# Make SDL2 available
FetchContent_MakeAvailable(SDL2)

# Configure SDL2_mixer to be self-contained
# Disable all external dependencies and use built-in decoders where possible
set(SDL2MIXER_SAMPLES OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_INSTALL OFF CACHE BOOL "" FORCE)

# Disable optional external libraries
set(SDL2MIXER_OPUS OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_FLAC OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_MOD OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_MIDI OFF CACHE BOOL "" FORCE)

# Use built-in/vendored libraries instead of system ones
set(SDL2MIXER_OPUS_SHARED OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_FLAC_LIBFLAC_SHARED OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_MOD_MODPLUG_SHARED OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_MIDI_FLUIDSYNTH_SHARED OFF CACHE BOOL "" FORCE)

# Enable only the formats we need for Wolfenstein 3D
# Wolf3D primarily uses OPL/AdLib music and digital sound effects
set(SDL2MIXER_WAVE ON CACHE BOOL "" FORCE)  # For sound effects
set(SDL2MIXER_OGG ON CACHE BOOL "" FORCE)   # Optional, uses stb_vorbis (built-in)
set(SDL2MIXER_OGG_STB ON CACHE BOOL "" FORCE)  # Use stb_vorbis instead of libogg
set(SDL2MIXER_MP3 OFF CACHE BOOL "" FORCE)  # Not needed for Wolf3D

# Alternatively, if you need more audio format support with vendored libraries:
# set(SDL2MIXER_VENDORED ON CACHE BOOL "" FORCE)  # Use all vendored libraries

FetchContent_MakeAvailable(SDL2_mixer)

# =============================================================================
# SOURCE FILES
# =============================================================================

# Core source files (common to all platforms)
set(CORE_SOURCES
    # ID Software components
    src/id_ca.cpp
    src/id_in.cpp
    src/id_pm.cpp
    src/id_sd.cpp
    src/id_us_1.cpp
    src/id_vh.cpp
    src/id_vl.cpp
    
    # Wolfenstein game logic
    src/wl_act1.cpp
    src/wl_act2.cpp
    src/wl_agent.cpp
    src/wl_debug.cpp
    src/wl_draw.cpp
    src/wl_game.cpp
    src/wl_inter.cpp
    src/wl_main.cpp
    src/wl_menu.cpp
    src/wl_play.cpp
    src/wl_state.cpp
    src/wl_text.cpp
    
    # Wolfenstein graphics effects
    src/wl_atmos.cpp
    src/wl_cloudsky.cpp
    src/wl_dir3dspr.cpp
    src/wl_floorceiling.cpp
    src/wl_parallax.cpp
    src/wl_shade.cpp
    
    # IOAN components (custom additions)
    src/ioan_bas.cpp
    src/ioan_bot.cpp
    src/ioan_secret.cpp
    
    # Data structures and utilities
    src/ActorStates.cpp
    src/CheckSum.cpp
    src/CommandLine.cpp
    src/Config.cpp
    src/DataFile.cpp
    src/DirectoryFile.cpp
    src/Exception.cpp
    src/e_hashkeys.cpp
    src/FileSystem.cpp
    src/HistoryRatio.cpp
    src/Logger.cpp
    src/MapExploration.cpp
    src/MasterDirectoryFile.cpp
    src/m_buffer.cpp
    src/obattrib.cpp
    src/PathArray.cpp
    src/Platform.cpp
    src/Property.cpp
    src/PropertyFile.cpp
    src/PString.cpp
    src/ShellUnicode.cpp
    src/SODFlag.cpp
    src/StdStringExtensions.cpp
    src/signon.cpp
    
    # HAL (Hardware Abstraction Layer)
    src/i_system.cpp
    src/i_video.cpp
    
    # DOSBox OPL emulation
    src/dosbox/dbopl.cpp
)

# Platform-specific sources
if(WIN32)
    list(APPEND PLATFORM_SOURCES
        src/win32/sdl_winmain.cpp
    )
    set(PLATFORM_RESOURCES
        win/Wolf4SDL.rc
    )
elseif(APPLE)
    list(APPEND PLATFORM_SOURCES
        src/macosx/SDLMain.m
        src/macosx/CocoaFun.mm
    )
    set(PLATFORM_RESOURCES
        macosx/wolf4sdl.icns
    )
endif()

# =============================================================================
# TARGETS
# =============================================================================

# Main executable
if(WIN32)
    add_executable(${PROJECT_NAME} WIN32
        ${CORE_SOURCES}
        ${PLATFORM_SOURCES}
        ${PLATFORM_RESOURCES}
    )
elseif(APPLE)
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE
        ${CORE_SOURCES}
        ${PLATFORM_SOURCES}
    )
    
    # Set bundle properties
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.ichera.autowolf"
        MACOSX_BUNDLE_BUNDLE_NAME "AutoWolf"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.ichera.autowolf"
        RESOURCE "${PLATFORM_RESOURCES}"
    )
    
    # Copy resources to bundle
    set_source_files_properties(${PLATFORM_RESOURCES} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )
else()
    add_executable(${PROJECT_NAME}
        ${CORE_SOURCES}
        ${PLATFORM_SOURCES}
    )
endif()

# =============================================================================
# INCLUDE DIRECTORIES
# =============================================================================

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# =============================================================================
# COMPILE DEFINITIONS
# =============================================================================

# Platform-specific definitions
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        WIN32
        _CRT_SECURE_NO_WARNINGS
        _CRT_NONSTDC_NO_WARNINGS
        _WINDOWS
    )
elseif(APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        APPLE_NON_CPP11  # Based on Xcode project settings
    )
endif()

# Debug definitions
if(AUTOWOLF_DEBUG_MAX OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        __AUTOWOLF_DEBUG_MAX_OS_X__
        _DEBUG
    )
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        NDEBUG
    )
endif()

# =============================================================================
# LINK LIBRARIES
# =============================================================================

target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL2::SDL2
    SDL2::SDL2main
    SDL2_mixer::SDL2_mixer
)

# Platform-specific libraries
if(APPLE)
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(IOKIT_LIBRARY IOKit REQUIRED)
    find_library(CARBON_LIBRARY Carbon REQUIRED)
    find_library(AUDIOUNIT_LIBRARY AudioUnit REQUIRED)
    find_library(AUDIOTOOLBOX_LIBRARY AudioToolbox REQUIRED)
    find_library(OPENGL_LIBRARY OpenGL REQUIRED)
    
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${CARBON_LIBRARY}
        ${AUDIOUNIT_LIBRARY}
        ${AUDIOTOOLBOX_LIBRARY}
        ${OPENGL_LIBRARY}
    )
elseif(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        opengl32
        winmm
    )
endif()

# =============================================================================
# INSTALLATION
# =============================================================================

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)

# =============================================================================
# SUGGESTED SUBDIRECTORY STRUCTURE
# =============================================================================

# You could split this CMakeLists.txt into multiple files by creating subdirectories:
#
# 1. src/CMakeLists.txt - For core game sources
#    add_subdirectory(src)
#
# 2. src/dosbox/CMakeLists.txt - For DOSBox OPL emulation
#    Create a static library for the DOSBox components
#
# 3. src/ioan/CMakeLists.txt - For IOAN-specific components
#    Group all IOAN-related files (ioan_*.cpp, data structures, etc.)
#
# 4. src/hal/CMakeLists.txt - For hardware abstraction layer
#    Group i_system.cpp, i_video.cpp
#
# 5. external/CMakeLists.txt - For managing external dependencies
#    Move FetchContent declarations here
#
# Example of how to structure src/CMakeLists.txt:
# 
# # In main CMakeLists.txt:
# add_subdirectory(src)
# add_subdirectory(external)
#
# # In src/CMakeLists.txt:
# add_library(autowolf_core STATIC
#     id_ca.cpp
#     id_in.cpp
#     # ... other core files
# )
# target_include_directories(autowolf_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# 
# add_subdirectory(dosbox)
# add_subdirectory(ioan)
# add_subdirectory(hal)
#
# # Then in main CMakeLists.txt, link these libraries:
# target_link_libraries(${PROJECT_NAME} PRIVATE
#     autowolf_core
#     autowolf_dosbox
#     autowolf_ioan
#     autowolf_hal
# )