# Cubosphere [reborn]
[![CircleCI (all branches)](https://img.shields.io/circleci/project/github/cubosphere/cubosphere-code.svg)](https://circleci.com/gh/cubosphere/cubosphere-code) [![#cubosphere on the freenode IRC network](https://img.shields.io/badge/FreeNode-%23cubosphere-brightgreen.svg)](https://webchat.freenode.net/?channels=cubosphere)
## A little story behind it
One upon a time on planet called Earth one man have found very very cool but anadobed in 2011-12 years game called "cubosphere". He enjoyed it and dreamt that some day it will become great.

And after a while he decided to pick it up and at least make look like a modern project, so others will be able to contribute easily.

This is why this organisation exists now.

# What is this game about?
Goal of this project is a freeware game similar to the PSX game "Kula World" / "Roll Away". It is designed platform independent, written in C++ and using the following libraries: OpenGL, SDL (Simple DirectMedia Layer), libjpeg, Lua, GLSL-Shader.

## Features
* Game similar to Kula World
* Over 450 levels in 34 different designs!
* Starting from beta 0.3 — a lot of new stuff like magnets, gravity changers and so on
* Multiball feature and two-player-mode
* Internal level editor!
* 3d-Engine via OpenGL
* GLSL-Shaders
* Joystick/Gamepad support

# What have I already done to improve the game?
* Migrated to CMake
* Separated data and code repos with auto ZIP
* Fixed few warnings
* Removed `using namespace std;`
* Made code C++17 compatible (no either warnings or errors)
* Configured Cricle CI with auto .deb build for Ubuntu LTS + last release (read next section)
* Created icon and .desktop file

# How to download last git build for Ubuntu?
1. Go to http://cubosphere.vallua.ru/artifacts (warning: only last four artifacts are kept)
2. Download apporative package for you (naming is Cubosphere-timestamp+git-hash-distribution-Linux.deb)
3. Install as normal (if you don't know how, just open file and click "Install" button or someting like it)

# What will/should be done?
Check out [TODO list](TODO.md)!

# How can I take part in fun and make cubosphere great again?
You can:

* Fork repo and make pull request or
* If you have made a lot of contributions and I trust you then you can ask for direct write access

# Ok, some credits
You can find original homepage, codebase and developers at https://sourceforge.net/projects/cubosphere.
