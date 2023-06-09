#----------------------------------------------------------------------------------------------------------------------
# Project config
#----------------------------------------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.21)
project(sdl2dsu 
        VERSION 1.0.2
        DESCRIPTION "A simple application that map SDL input to DSU protocol (cemuhook)."
        HOMEPAGE_URL "https://github.com/FrogTheFrog/sdl2dsu"
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(STATIC_BUILD OFF CACHE BOOL "Use static linking")

if(STATIC_BUILD)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    set(BUILD_SHARED_LIBS OFF)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
endif()

#----------------------------------------------------------------------------------------------------------------------
# Setup SDL
#----------------------------------------------------------------------------------------------------------------------

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(SDL_AUDIO OFF)
set(SDL_VIDEO OFF)
set(SDL_RENDER OFF)
set(SDL_SHARED OFF)
set(SDL_STATIC ON)
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
    add_compile_definitions(_USE_MATH_DEFINES)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

#----------------------------------------------------------------------------------------------------------------------
# Subdirectories
#----------------------------------------------------------------------------------------------------------------------

add_subdirectory(src)
