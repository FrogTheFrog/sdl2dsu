# sdl2dsu

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

1. Take a look at `scripts/sdl2dsu.service` and modify the `ExecStart` to your needs.
2. Copy it to one of the systemd directories. For example `sudo cp scripts/sdl2dsu.service /etc/systemd/system/sdl2dsu.service`.
3. Refresh the daemon and enable the service - `sudo systemctl daemon-reload && sudo systemctl enable sdl2dsu --now`.
