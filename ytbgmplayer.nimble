# Package

version       = "1.3.1"
author        = "WeebNetsu"
description   = "A lightweight music player for playing YouTube music in the background."
license       = "MIT"
srcDir        = "src"
bin           = @["ytbgmplayer"]


# Dependencies

requires "nim >= 2.2.4"
requires "cligen >= 1.5.23"
requires "mpv >= 0.2.0"
requires "ncurses >= 1.0.2"
