#----------------------------------------------------------------------------------------------------------------------
# Project config
#----------------------------------------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.21)
project(sdl2dsu 
        VERSION 1.0.0
        DESCRIPTION "A simple application that map SDL input to DSU protocol (cemuhook)."
        HOMEPAGE_URL "https://github.com/FrogTheFrog/sdl2dsu"
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# This app can be easily built statically on Windows only. On other systems it's more complicated...
#if(WIN32)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    set(BUILD_SHARED_LIBS OFF)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
#endif()

#----------------------------------------------------------------------------------------------------------------------
# Setup SDL
#----------------------------------------------------------------------------------------------------------------------

# See https://github.com/bincrafters/community/issues/696
add_compile_definitions(SDL_VIDEO_DRIVER_X11_SUPPORTS_GENERIC_EVENTS=1)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(SDL_STATIC ON)
set(SDL_SHARED OFF)
set(SDL_ALSA_SHARED OFF)
set(SDL_JACK_SHARED OFF)
set(SDL_PIPEWIRE_SHARED OFF)
set(SDL_PULSEAUDIO_SHARED OFF)
set(SDL_SNDIO_SHARED OFF)
set(SDL_X11_SHARED OFF)
set(SDL_WAYLAND_SHARED OFF)
set(SDL_WAYLAND_LIBDECOR_SHARED OFF)
set(SDL_KMSDRM_SHARED OFF)
add_subdirectory(external/sdl)

#----------------------------------------------------------------------------------------------------------------------
# Setup Boost
#----------------------------------------------------------------------------------------------------------------------

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.74.0 COMPONENTS log log_setup program_options REQUIRED)

#----------------------------------------------------------------------------------------------------------------------
# Compile settings
#----------------------------------------------------------------------------------------------------------------------

if(MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

#----------------------------------------------------------------------------------------------------------------------
# Subdirectories
#----------------------------------------------------------------------------------------------------------------------

add_subdirectory(src)
