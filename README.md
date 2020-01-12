# Sc2AutoObserver

## About
Starcraft 2 replays auto observer inspired by the [SSCAIT-ObserverModule](https://github.com/Plankton555/SSCAIT-ObserverModule).

## Build requirements
1. Download (the password is iagreetotheeula) at least one of the following map packs:
  * [Ladder 2019 Season 3](http://blzdistsc2-a.akamaihd.net/MapPacks/Ladder2019Season3.zip)

2. Put the downloaded maps into the Maps folder (create it if the folder doesn't exist):
  * Windows: C:\Program Files\StarCraft II\Maps
  * OS X: /Applications/StarCraft II/Maps

3. Download and install [CMake](https://cmake.org/download/).

4. A compiler with C++17 support.

5. Windows: Download and install [Visual Studio](https://www.visualstudio.com/downloads/)

6. OS X: Install XCode.

## Build instructions

### Windows (Visual Studio)
```bat
:: Clone the project.
$ git clone --recursive git@gitlab.com:aiarena/sc2autoobserver.git
$ cd sc2autoobserver

:: Create build directory.
$ mkdir build
$ cd build

:: Generate VS solution.
$ cmake ../ -G "Visual Studio 15 2017 Win64"

:: Build the project using Visual Studio.
$ start Observer.sln

:: Launch the observer.
$ bin/Observer.exe --Path "<Path to a single SC2 replay or directory with replay files>" --Speed <Replay speed>`
```

### OS X (Xcode)
```bash
# Clone the project.
$ git clone --recursive git@gitlab.com:aiarena/sc2autoobserver.git && cd sc2autoobserve

# Create build directory.
$ mkdir build && cd build

# Generate a Makefile.
# Use 'cmake -DCMAKE_BUILD_TYPE=Debug ../' if debuginfo is needed
# Debug build also contains additional debug features and chat commands support.
$ cmake ../ -G Xcode

# Build the project using Xcode.
$ open Observer.xcodeproj/

# Launch the observer.
$ ./bin/Observer --Path "<Path to a single SC2 replay or directory with replay files>" --Speed <Replay speed>`
```

## License
Copyright (c) 2017 Daniel K�hntopp

Licensed under the [MIT license](LICENSE).
