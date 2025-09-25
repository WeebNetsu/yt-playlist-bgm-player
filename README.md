# YouTube Playlist BGM Player

This is a basic background music player made with Nim to play YouTube Playlists in the background. It is focused on being very light weight and easily accessible, even if you don't have a Desktop Environment or Window Manager.

Looking for the C++ version? [Click here](https://github.com/WeebNetsu/yt-playlist-bgm-player/tree/cpp). Take note that it is no longer being developed.

## Content

- [Running and Installing](#running-and-installing)
  - [Install](#install)
  - [Run](#run)
- [How to update](#how-to-update)
- [Compiling the code](#compiling-the-code)
- [Controls](#controls)
- [The playlist file](#the-playlist-file)
- [Features](#features)
  - [Playing Playlists](#playing-playlists)
  - [Adding Playlists](#adding-playlists)
  - [Editing Playlists](#editing-playlists)
  - [Deleting Playlists](#deleting-playlists)
  - [Shuffling Playlists](#shuffling-playlists)
- [Flags](#flags)

## Running and Installing

_Note: You should have a stable internet connection to use YouTube playlist player with YouTube playlists._

### Install

#### AUR

`paru -S ytbgmplayer-git`

#### From Source

Clone or download the files run this in the project root directory: `nimble install`

### Run

There are 2 ways to run this software, with or without flags. Without any flags it will provide a CLI you can use to play your playlists, with flags it will immediately start playing playlists.

#### Example 1

`ytbgmplayer` - this will provide the CLI

#### Example 2

`ytbgmplayer 1 2 --shuffle --loop` - this will play playlist nr 1 and 2, shuffle them and loop them.
`ytbgmplayer 1 2 -sl` - this is the short version of the above

#### Example 3

`ytbgmplayer --list` - list all available playlists, if this flag is used, no other flags are allowed

## How to update

### AUR

You can use whichever AUR package manager you prefer to update the package

### From Source

Follow all the steps as in [install](#from-source).

## Compiling the code

If you want to compile the source code yourself, you'll need the dependencies listed below.

To compile the code (for release): `nimble build -d:release`
To compile the code (for debugging): `nimble build`
To run the code (debugging): `nimble run`

### Dependencies

- [Nim](https://nim-lang.org) _(only if compiling from source)_
- [Nimble](https://github.com/nim-lang/nimble) _(only if compiling from source)_
- The <b>latest</b> version of [mpv](https://mpv.io/installation/). (Debian/Ubuntu-based users will have to add a repository and install the latest version: `add-apt-repository ppa:mc3man/mpv-tests -y`)
- [yt-dlp](https://github.com/yt-dlp/yt-dlp)

## Controls

| Action            | Key         |
| ----------------- | ----------- |
| Pause             | Space       |
| Mute              | m           |
| Skip 10 Seconds   | Right Arrow |
| Rewind 10 Seconds | Left Arrow  |
| Skip 1 Minute     | Up Arrow    |
| Rewind 1 Minute   | Down Arrow  |
| Next Song         | >           |
| Previous Song     | <           |
| Quit              | q           |

## The Playlist File

The playlist file (also known as the save file) is stored in `$HOME/.config/ytbgmpcli/playlists.json`, this is where all the playlists and their related details are stored.

## Features

### Playing Playlists

You can play playlists from either on YouTube or your current machine (locally).

### Adding Playlists

You can add both playlists from YouTube and from your machine. To add a playlist from YouTube, just copy the link to the playlist and when you have to insert the playlist link in th "Add Playlist" option, just paste the link. When you want to add a playlist from your machine (locally), create a folder, put all the music in there that you want in the playlist, navigate your terminal to that folder (cd /path/to/folder), when inside the folder type the command "pwd" and copy the output. When you have to add the location of the file in "Add Playlist", just paste the copied output into the input. Done.

### Editing Playlists

You can edit your playlist details, such as the playlist name and the playlist location/link.

### Deleting Playlists

Once you have listened too much to a playlist, it is time to delete it. You can easily do that by choosing the delete option.

### Shuffling Playlists

If playing the playlist in it's normal order (YouTube: The way you ordered the playlist | Local: A-Z) then you can shuffle it and enjoy yourself.

## Flags

If you don't want to use the interface, you can use flags to do some of the things you need to do.

### The playlist flag

This flag is used to run a playlist without opening the interface, this can be done by running ytbgmplayer with the playlist number afterwards. This should always be added when using multi-answer flags.<br>Example of the playlist flag: <code>bgmplayer 2</code> (will play playlist nr. 2)<br>Another example of the playlist flag: <code>bgmplayer 1 2</code> (will play playlist nr. 1 & 2)

### All other flags

| Flag      | Short | Action                                  |
| --------- | ----- | --------------------------------------- |
| --help    | -h    | Displays help                           |
| --list    | N/A   | Displays all playlists                  |
| --random  | -r    | Play all your playlists in random order |
| --shuffle | -s    | Shuffle playlists                       |
| --loop    | -l    | Loop playlists                          |

---

If you want to support the work I do, please consider donating to me on one of these platforms:

[<img alt="liberapay" src="https://img.shields.io/badge/-LiberaPay-EBC018?style=flat-square&logo=liberapay&logoColor=white" />](https://liberapay.com/stevesteacher/)
[<img alt="kofi" src="https://img.shields.io/badge/-Kofi-7648BB?style=flat-square&logo=ko-fi&logoColor=white" />](https://ko-fi.com/stevesteacher)
[<img alt="paypal" src="https://img.shields.io/badge/-PayPal-0c1a55?style=flat-square&logo=paypal&logoColor=white" />](https://www.paypal.com/donate/?hosted_button_id=P9V2M4Q6WYHR8)
