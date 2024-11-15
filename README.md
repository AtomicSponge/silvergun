#  Silvergun Game Engine

__Silvergun__ is a lightweight cross-platform game engine written in C++17 using an [ECS](https://en.wikipedia.org/wiki/Entity_component_system) design.

### Requirements
 - __Build tools__:
    - A working C++ build environment with [CMake](https://cmake.org)
 - __Libraries__:
    - [Allegro Game Library](https://liballeg.org)
    - [OpenGL](https://www.opengl.org) *(2d only)*

-----

## Library Installation

Installation is handled by [CMake](https://cmake.org/).  To download and prepare the library for installation run:
```
git clone https://github.com/AtomicSponge/silvergun.git
cd silvergun
mkdir build
cd build
cmake ..
cd ..
```

Then to install the library run:
```
sudo cmake --build build --target install --config Build
```

-----

## Demo Game

<https://github.com/AtomicSponge/slv-demo-01>
