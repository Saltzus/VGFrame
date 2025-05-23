# Duelike

Duelike is a multiplayer deathmatch point & click roguelike game

## Client
The client is built with C++

#### Libraries
- [Raylib](https://github.com/raysan5/raylib)
- [asio (non boost)](https://github.com/chriskohlhoff/asio)
- [OpenSSL](https://github.com/janbar/openssl-cmake)

## Server

The server is built with Go (Golang)

## Setup instructions

### Requirements
- [Cmake (minimum 3.12)](https://cmake.org/download/)
- [GO](https://go.dev/doc/install)

#### Windows
- [Visual Studio 2022](https://visualstudio.microsoft.com/vs/community/)

#### Linux
```
sudo apt-get update && sudo apt-get install -y libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev
```

### Building 
- Create ```build``` folder
- Open terminal inside ```build```
- Run command ```cmake ..```
