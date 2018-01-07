STEN (KURT)
============

STEN is supported on both Windows, Mac and Linux.

Building
--------

This library uses [CMake](https://cmake.org/download/) to generate project files. It builds with [Visual Studio](https://www.visualstudio.com/downloads/) on Windows and Makefiles on the other platforms. It relies on the following contrib packages. 

1. Civetweb
2. Protobuf
3. (optional) SDL

You need to download StarCarft II before building. On Windows and Mac you download StarCraft II the normal way and you need to run it once. On Linux you can download StarCraft II from [s2client-proto](https://github.com/Blizzard/s2client-proto).

Follow the instructions for building in [docs/building.md](docs/building.md).

To start STEN
-------------------
On Windows (Visual Studio) and Mac (Xcode) you build and run the solution called “tutorial”.

On Linux you run the executable feature_layers in build/bin. You also need to specify the path to StarCraft II with the -e flag.
