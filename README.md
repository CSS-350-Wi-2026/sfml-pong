# SFML Pong — Built with the CMake SFML Project Template

A modern Pong implementation using the [SFML](https://www.sfml-dev.org/) multimedia library and a [CMake](https://cmake.org/)-based build system.

---

# Documentation Index

Jump to the section you need.

## Game Documentation
- [Overview](#overview)
- [Game Description](#game-description)
- [How to Play](#how-to-play)
- [System Requirements](#system-requirements)

## Build System and Template Documentation
- [Quick Start](#quick-start)
- [Install and Run](#install-and-run)
- [Upgrading SFML](#upgrading-sfml)
- [Advanced CMake Options](#advanced-cmake-options)

## Additional Information
- [More Reading](#more-reading)
- [License](#license)
- [Acknowledgements](#acknowledgements)

---

# Game Documentation

## Overview

SFML Pong is a modernized version of the classic arcade Pong game built using the  
[SFML (Simple and Fast Multimedia Library)](https://www.sfml-dev.org/).

The project focuses on:

- Smooth gameplay
- Simple but extendable code architecture
- Clear separation between game logic and the build system
- Cross-platform compatibility

---

## Game Description

The game recreates the classic Pong experience with several modern improvements.

Features include:

- Single-player mode with AI opponent
- Increasing ball speed as rallies continue
- Paddle-velocity-based ball deflection
- Smooth AI paddle movement
- Customizable background color
- Adjustable FPS and audio volume
- Settings menu accessible during gameplay

The codebase is designed to remain simple and easy to extend with additional features.

---

## System Requirements

Minimum requirements:

- 64-bit CPU
- 2 GB RAM
- Display with **800×600 resolution**
- Audio output device
- A C++20-compatible compiler

Required tools:

- [Git](https://git-scm.com/)
- [CMake](https://cmake.org/) 3.16 or newer

---

## How to Play

| Key | Action |
|----|----|
| **W / S** or **↑ / ↓** | Move paddle |
| **ESC** | Pause or resume |
| **TAB** | Open settings |
| **SPACE / ENTER** | Start or restart |

Gameplay becomes progressively harder as the ball speed increases during long rallies.

---

# Build System and Template Documentation

## Quick Start

This project uses the official  
[CMake SFML Project Template](https://github.com/SFML/cmake-sfml-project).

### 1. Install Required Tools

Install the following software:

- Git — https://git-scm.com/downloads
- CMake — https://cmake.org/download/

### 2. Create a Repository From the Template

Use GitHub’s template feature:

https://docs.github.com/en/repositories/creating-and-managing-repositories/creating-a-repository-from-a-template

Template repository:

https://github.com/SFML/cmake-sfml-project

### 3. Clone the Repository

```bash
git clone <your-repo-url>
cd <repo>
```

### 4. Modify the Project Name

Open **CMakeLists.txt** and update:

- the `project()` name
- the executable target name

### 5. Add Source Files

Modify the `add_executable()` call:

```cmake
add_executable(game
    main.cpp
    pong.cpp
)
```

### 6. Link SFML Modules

```cmake
target_link_libraries(game
    PRIVATE
        SFML::Graphics
        SFML::Audio
        SFML::Network
)
```

---

## Linux Dependencies

Install required packages:

```bash
sudo apt update
sudo apt install \
    libxrandr-dev \
    libxcursor-dev \
    libxi-dev \
    libudev-dev \
    libfreetype-dev \
    libflac-dev \
    libvorbis-dev \
    libgl1-mesa-dev \
    libegl1-mesa-dev
```

---

## Build the Project

```bash
cmake -B build
cmake --build build
```

The executable will appear in the `build/` directory.

Run it with:

```bash
cd build
./main
```

---

## Install and Run

### Windows

1. Clone the repository.
2. Navigate to `src/Main`.
3. Run `main.exe`.

### macOS / Linux

Compile and run:

```bash
g++ main.cpp -o main
./main
```

Or use the CMake build system:

```bash
cmake -B build
cmake --build build
cd build
./main
```

---

## Upgrading SFML

The project uses **CMake FetchContent** to automatically download SFML.

Edit `CMakeLists.txt`:

```cmake
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        3.0.2
)
```

Change the `GIT_TAG` value to upgrade or downgrade the library version.

---

## Advanced CMake Options

### Change Compiler

You can specify a compiler with:

```bash
cmake -B build -DCMAKE_CXX_COMPILER=clang++
```

More information:

https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER.html

---

### Build Types

| Type | Description |
|-----|-------------|
| Debug | Debug symbols, no optimization |
| Release | Optimized build |

Example:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
```

---

### Change Generator

Example using the Ninja generator:

```bash
cmake -G "Ninja" -B build
cmake --build build
```

Generator documentation:

https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html

---

# More Reading

- Official CMake Tutorial  
  https://cmake.org/cmake/help/latest/guide/tutorial/

- How to Use CMake Without the Agonizing Pain – Part 1  
  https://alexreinking.com/blog/how-to-use-cmake-without-the-agonizing-pain-part-1.html

- How to Use CMake Without the Agonizing Pain – Part 2  
  https://alexreinking.com/blog/how-to-use-cmake-without-the-agonizing-pain-part-2.html

- Better CMake YouTube Series  
  https://www.youtube.com/playlist?list=PL8i3OhJb4FNV10aIZ8oF0AA46HgA2ed8g

---

# License

The source code is dual-licensed under:

- MIT License
- Public Domain

You may choose whichever license you prefer.

---

# Acknowledgements

- [SFML — Simple and Fast Multimedia Library](https://www.sfml-dev.org/)
- [CMake SFML Project Template](https://github.com/SFML/cmake-sfml-project)
- The SFML developers and contributors.