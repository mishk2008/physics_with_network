# Moving balls

A client-server application: the server simulates a physical world of balls, and the Qt client connects over TCP and displays their motion.

The project builds into two executables:

- `server.exe` - the server;
- `client.exe` - the client with a graphical interface.

## System requirements and dependencies

- **OS:** Windows 10/11 (testing was done on Windows).
- **Compiler:** MSYS2 with GCC (C++20 standard).
- **Build system:** CMake 3.20 or newer, Ninja generator.
- **Libraries:** Qt 6 (Core, Network, Widgets modules) and the header-only Asio library.

All dependencies can be installed with the MSYS2 package manager (`pacman`).

## Building

1. Open an **MSYS2 MINGW64** terminal.
2. Go to the project root directory (the one containing `CMakeLists.txt`).
3. Create a build directory and enter it:

   ```bash
   mkdir build && cd build
   ```

4. Generate the build files:

   ```bash
   cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
   ```

5. Compile:

   ```bash
   ninja
   ```

After a successful build, `server.exe` and `client.exe` will appear in the `build` directory.

## Running the server

The server is started from the console. It uses port `12345` by default; a different port can be passed as a command-line argument:

```bash
./server.exe 8080
```

On startup the server initializes the physical world (10 balls by default) and prints its local network IP address and the port to connect to.

### Console controls

The server supports an interactive mode (the control thread runs in parallel with the simulation):

| Command | Action |
|---------|--------|
| `0` | graceful server shutdown |
| `1` | remove the last added ball (LIFO) |
| `2` | add a ball with random position, velocity and mass |
| `3` | add a ball with manually entered parameters |

Parameter format for command `3`:

```
<x>;<y>;<vel_x>;<vel_y>;<radius>;<mass>
```

## Running the client

Start the client by double-clicking it or from the console:

```bash
./client.exe
```

### Connecting to a server

1. Enter the server IP address in the `Host:` field of the top panel (for example, `127.0.0.1` for the local machine, or the server's address on the local network).
2. Enter the port number in the `Port:` field (default `12345`).
3. Click `Connect`. If the address is correct and the server is running, the status in the top-right corner changes to `Status: Connected` and the balls start moving in the window.
4. Click `Disconnect` to close the connection.

### Additional features

The `Show balls parameters` checkbox enables a debug mode that draws each ball's ID, velocity, mass and coordinates next to it. To avoid performance loss, text drawing is turned off automatically when there are more than 100 balls.
