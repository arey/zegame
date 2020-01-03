NAINS - ZEGAME
==============

                   Computer Role Playing Game                          
                         
                         Version 1.75
                    (C) Copyright 1998-2001
                   Graphics by Joël Mouclier
                       Code by Antoine Rey
                   Music by Guy-Roger Duvert

Presentation
------------

ZeGame is a single 3D isometric role playing game developed in C++ and x86 assembly language in the late 90's.
It will immerse you in a full gaming experience in an epic fantasy virtual world.
The story takes place a long time ago in a medieval world where some inhabitant have the secret of strange kind of steam engines of our late XIXth century.


This Git repository contains:
 - [binaries](/releases/tag/v1.75): the zip archives of the game binaries and its tools/editors
 - [docs](docs): ZeGame's website available at [https://arey.github.io/zegame/](https://arey.github.io/zegame/),
 including the documentation for game designer and developers
  - [sources](sources): the C++ source files of the game and its tooling (editors)
 - [music](music): 2 original musics composed by [Guy-Roger Duvert](http://www.grduvert.com/)

![ZeGame screenshot](docs/bitmap/screen-beta4.jpg?raw=true)

Game Features
-------------

- Powerful graphic engine in 3D isometric: transluency, shadows, infinite levels, ...
- A large set of character animations: walk, run, jump, hit, ...
- Fairy musical atmosphere and immersive sound effects
- 2 full levels right now available


System Requirements
-------------------

The game has initially been developed for Microsoft Windows® 95/98 and Windows 2000.
However, it could be launched on Windows 7 and Windows 10.
On Windows 10, the background could be black.

Nowadays, all computers have the minimum system configuration required to play :
- Intel Pentium® 200 or 100% compatible CPU (PII 300 recommended)
- 64 Mb RAM (96 Mb recommended)
- 2 MbVideo card accelerated 2D (8Mb is better)
- 50 MB available hard drive space
- Microsoft Windows® 95/98 or 2000
- DirectX™ 6.1 or higher


Installation
------------
All binaries could be download from the [v.175 release](/zegame/releases/tag/v1.75).

Download then unzip the [ZeGame-v1.75.zip](https://github.com/arey/zegame/releases/download/v1.75/ZeGame-v1.75.zip) archive file.
Execute the `ZeGame.exe` binary.


DirectPlay is an older API library that was a component of earlier DirectX versions.
Some older games like ZeGame don't run without DirectPlay.
On Windows 10, DirectPlay is enabled at the first startup of ZeGame.

Keyboard controls
-----------------

| Key                | Description |
|  :---:             | :---        |
| Escape             | Quit ZeGame | 
| F1                 | Display a succinct Help | 
| F2                 | Show the current frame rate | 
| +/-                | Modify the frame rate | 
| Arrow Keys         | Move your character | 
| Left Shift         | Run | 
| W key (+direction) | Jump | 
| Left Ctrl          | Hit somebody or something | 
| Space              | Use/See/Speak | 
| Left Alt           | Change active weapon | 
| 1,2,3              | Select a weapon | 
| K                  | Die or resurrect | 
