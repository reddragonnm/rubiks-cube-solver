# Rubik's Cube Solver

![Demo](videos/sample.gif)

- A 3D 3x3 Rubik's Cube rendered from scratch in an SFML window.  
- It uses Kociemba's algorithm to solve the cube in less than 30 moves.

## Features

- Real-time 3D rendering of the Rubik's Cube
- Interactive cube manipulation via keyboard
- Shuffle and solve functionality
- Visual animation of rotations and solution steps
- Uses SFML for graphics and input

## Controls

- Arrow keys: Rotate the cube view
- U/D/L/R/F/B: Rotate cube faces (Up, Down, Left, Right, Front, Back)
- Space: Shuffle the cube
- Enter: Solve the cube (when idle)

## Build Instructions

1. Install [CMake](https://cmake.org/) and a C++17 compiler.
2. Clone the repository and initialize submodules if needed.
3. Run:
   ```sh
   cmake -S . -B build
   cmake --build build
   ```
4. Run the executable from `build/bin/`.

## Project Structure

- `src/`: Source code (`main.cpp`, `Cube.hpp`, `Camera.hpp`, `Solver.hpp`, etc.)
- `videos/`: Demo GIFs and videos
- `build/`: Build output
- `.github/`: CI workflows

## Algorithm

The solver uses a two-phase approach inspired by Kociemba's algorithm:
- **Phase 1:** Reduces the cube to a subset of states using IDA* search.
- **Phase 2:** Solves the cube from the reduced state using further IDA* search.

Move tables and pruning tables are generated and loaded for efficient searching.

## License

See [LICENSE.md](LICENSE.md) for details.
