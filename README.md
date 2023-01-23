## Pop or Fail

![Pop or Fail](readme-images/readme-image000.png "Pop or Fail")

### Description

A simple game written in C using [Raylib](https://raylib.com) for [20 Second Game Jam](https://itch.io/jam/20-second-game-jam)

You have to pop the circles that match the color of the circle at the top.

### Controls

 - `S` while in the menu toggles sound
 - `LMB` for everything else

### Screenshots
<br>
<img src="./readme-images/readme-image001.gif" width="288px">

### Developers

 - [degradka](https://github.com/degradka) - UI, Game logic
 - [mamalord](https://github.com/mamalord) - Code optimization, Game logic

### Play the game

 - gitlab.com Releases: [Releases](https://gitlab.com/pop-or-fail/pop-or-fail/-/releases)
 - github.com Releases: [Releases](https://github.com/degradka/pop-or-fail/releases)
 - itch.io Release: [Game Page](https://degradka.itch.io/pop-or-fail)

### Building the game
#### Building for GNU/Linux
- `sudo apt install build-essentials` installs the libraries
- [Follow the instructions in the Raylib repo](https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux) to compile it
- `git clone https://github.com/degradka/pop-or-fail.git` clones the repo
- `cd pop-or-fail/src` cds to the source directory
- `make BUILDTYPE=RELEASE` builds the game in release mode
- `./pop-or-fail` runs the game

#### Building for Windows 
- Install [Cygwin](https://www.cygwin.com) or [MSYS2](https://www.msys2.org)
- Make sure to install the required packages (`gcc`, `make`)
- [Follow the instructions in the Raylib repo](https://github.com/raysan5/raylib/wiki/Working-on-Windows) to compile it
- `git clone https://github.com/degradka/pop-or-fail.git` clones the repo
- `cd pop-or-fail/src` cds to the source directory
- `make BUILDTYPE=RELEASE` builds the game in release mode
- `./pop-or-fail.exe` runs the game

---
### License

This game sources are licensed under an unmodified zlib/libpng license, which is an OSI-certified, BSD-like license that allows static linking with closed source software. Check [LICENSE](LICENSE) for further details.
