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

#----------------------------------------------------------------------------------------------------------------------
# Setup SDL
#----------------------------------------------------------------------------------------------------------------------

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(SDL_SHARED OFF)
set(SDL_STATIC ON)
add_subdirectory(external/sdl)

if(UNIX)
    set(MANDATORY_OPTS
        DBUS
        HIDAPI
        HIDAPI_JOYSTICK
        IBUS
        LIBUDEV
        VIRTUAL_JOYSTICK
        X11_XINPUT
    )
else()
    set(MANDATORY_OPTS)
endif()

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
