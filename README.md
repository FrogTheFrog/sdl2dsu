# sdl2dsu ![Status](https://github.com/FrogTheFrog/sdl2dsu/actions/workflows/publish.yaml/badge.svg)

A simple application that map SDL input to DSU protocol (cemuhook).

# Build instructions

1. `git clone https://github.com/FrogTheFrog/sdl2dsu.git`
2. `cd sdl2dsu`
3. `git submodule update --init --recursive`
4. `cmake -DCMAKE_BUILD_TYPE:STRING=Release -G Ninja -B ./build`
5. `cmake --build build`

Binary will be located in `build/src`.

# Running the app

Run the app with `sdl2dsu --help` for more info.

# Installing as a service

Take a look in the `examples` directory on how the service should look like.
