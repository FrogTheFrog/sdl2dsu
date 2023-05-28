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
set(ENABLE_CLANG_TIDY OFF CACHE BOOL "Enable clang-tidy build (slow)")

#----------------------------------------------------------------------------------------------------------------------
# Setup SDL
#----------------------------------------------------------------------------------------------------------------------

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(SDL_DISABLED_SUBSYSTEMS Audio Video Render Power CPUinfo)
foreach(_SUB ${SDL_DISABLED_SUBSYSTEMS})
  string(TOUPPER ${_SUB} _OPT)
  set(SDL_${_OPT} OFF)
endforeach()

set(SDL_DISKAUDIO OFF)
set(SDL_DUMMYAUDIO OFF)
set(SDL_DUMMYVIDEO OFF)
set(SDL_OPENGL OFF)
set(SDL_OPENGLES OFF)
set(SDL_OSS OFF)
set(SDL_ALSA OFF)
set(SDL_JACK OFF)
set(SDL_PIPEWIRE OFF)
set(SDL_PULSEAUDIO OFF)
set(SDL_SNDIO OFF)
set(SDL_X11 OFF)
set(SDL_WAYLAND OFF)
set(SDL_RPI OFF)
set(SDL_ROCKCHIP OFF)
set(SDL_COCOA OFF)
set(SDL_DIRECTX OFF)
set(SDL_WASAPI OFF)
set(SDL_RENDER_D3D OFF)
set(SDL_RENDER_METAL OFF)
set(SDL_VIVANTE OFF)
set(SDL_VULKAN OFF)
set(SDL_METAL OFF)
set(SDL_KMSDRM OFF)
set(SDL_OFFSCREEN OFF)
set(SDL_SHARED OFF)
set(SDL_STATIC ON)
add_subdirectory(external/sdl)

#----------------------------------------------------------------------------------------------------------------------
# Setup Boost
#----------------------------------------------------------------------------------------------------------------------

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.80.0 COMPONENTS log log_setup program_options REQUIRED)

#----------------------------------------------------------------------------------------------------------------------
# CLang-Tidy
#----------------------------------------------------------------------------------------------------------------------

if(ENABLE_CLANG_TIDY)
    if(NOT MSVC)
        message(STATUS "Clang-tidy build is enabled!")
        set(CMAKE_CXX_CLANG_TIDY clang-tidy)
        file(WRITE "${CMAKE_BINARY_DIR}/.clang-tidy" "Checks: '-*,llvm-twine-local'")
    endif()
endif()

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
