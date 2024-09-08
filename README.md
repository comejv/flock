# Flocking Simulator

This project is a flocking simulator using the Raylib library. It demonstrates basic behaviors of entities (agents) in a simulation where they exhibit flocking behavior, such as grouping and repelling. It supports dynamic changes to the simulation parameters and real-time display of various metrics.

## Features

- **Entity Management**: Add and remove entities dynamically.
- **Simulation Parameters**: Adjust repulsion and group radii.
- **Real-time Metrics**: Display FPS, simulation energy, and entity count.
- **Interactive Controls**: Toggle various display options and modify parameters on the fly.

<p align="center">
<img src="https://github.com/user-attachments/assets/8f4703c3-a493-467d-89a3-36c3388d9ba7" width="500" height="500">
</p>

## Dependencies

- [Raylib](https://www.raylib.com/) - for graphics and window management.
- `gcc` - for compiling the C code.

## Compilation

To compile flock you should first compile raylib (change `PLUG` to what you want, ON by default):
```sh
make raylib PLUG=ON
```

### For Plugin Mode

To compile as a shared library with hot reload support, set `PLUG` to `ON` in the Makefile (default):

```sh
make PLUG=ON
```

### For Standalone Executable

To compile as a standalone executable, ensure `PLUG` is set to `OFF` in the Makefile (when compiling raylib too):

```sh
make PLUG=OFF
```

## Controls

- **F**: Toggle FPS display
- **E**: Toggle Energy display
- **N**: Toggle Entity Count display
- **P**: Add an entity (up to 1000 entities)
- **M**: Remove the last entity
- **I**: Toggle Radii Information display
- **UP Arrow**: Increase repulsion radius
- **DOWN Arrow**: Decrease repulsion radius (minimum: 10)
- **LEFT Arrow**: Decrease group radius (minimum: 10)
- **RIGHT Arrow**: Increase group radius
- **H**: Toggle Help display

## Project Structure

- **src/**: Contains source code files.
- **headers/**: Contains header files.
- **lib/raylib/src/**: Contains Raylib source files.
- **plug/**: Contains plugin source files (if using plugin mode).

## Makefile Targets

- `make all`: Compile the project.
- `make clean`: Remove object files.
- `make cleanall`: Remove all binaries and object files.
- `make raylib`: Compile Raylib into a static library.
- `make help`: Display help for available Makefile targets.

## License

This project is licensed under the GPL License. See the [LICENSE](LICENSE) file for details.
