# YouTube Playlist BGM Player

This is a basic background music player made with C++ to play YouTube Playlists in the background. It is focused on being very light weight and easily accessable, even if you don't have a Desktop Enviroment or Window Manager (so cli only).
Note: this only works on Linux, I'm planning to give it support for other OSs in the future.

## Content

<ul>
    <li><a href="#howToRun">How to run</a></li>
    <li><a href="#howToUpdate">How to update</a></li>
    <li><a href="#compilingTheCode">Compiling the code</a></li>
    <li><a href="#controls">Controls</a></li>
    <li><a href="#features">Features</a></li>
    <ul>
        <li><a href="#playingPlaylists">Playing Playlists</a></li>
        <li><a href="#addingPlaylists">Adding Playlists</a></li>
        <li><a href="#editingPlaylists">Editing Playlists</a></li>
        <li><a href="#deletingPlaylists">Deleting Playlists</a></li>
        <li><a href="#shufflingPlaylists">Shuffling Playlists</a></li>
    </ul>
    <li><a href="#flags">Flags</a></li>
    <ul>
        <li><a href="#thePlaylistFlag">The Playlist Flag</a></li>
        <li><a href="#singleAnswerFlags">Single-Answer Flags</a></li>
        <li><a href="#multiAnswerFlags">Multi-Answer Flags</a></li>
    </ul>
</ul>

<span id="howToRun"></span>

## How to run

<i>Note: You should have a stable internet connection to use the installer and YouTube playlist player.</i>

Clone or download the files and make sure the debian_based_installer.sh file is executable with:<br>
<code><pre>chmod +x debian_based_installer.sh</pre></code>
Run the debian_based_installer.sh file (debian only) and you're ready to run the application! To run the application, execute the following command in your terminal:
<code><pre>bgmplayer</pre></code>

If you don't have a Debian based linux, you can compile the code (or use the existing output file) and just mimic everything happening in the debian_based_installer.sh, but modified for your system (so not apt, but pacman or whatever package manager you use).

<span id="howToUpdate"></span>

## How to update

If the application has a new version, you can redownload the source code and run the debian_based_installer.sh file. It will update whatever should be updated. If you don't have a Debian based system, just move the output file to a bin folder or add it to path for the latest version (you can compile it yourself if you want)

<span id="compilingTheCode"></span>

## Compiling the code

If you want to compile the source code yourself, you'll need the dependencies and g++.
To compile: <code>g++ main.cpp FileManager.cpp -o output</code>

<span id="dependencies"></span>

### Dependencies

<ul>

<li><a href="https://www.geeksforgeeks.org/compiling-with-g-plus-plus/">g++</a> or some other C++ compilier</li>
<li>The <b>latest</b> version of <a href="https://mpv.io/installation/">mpv</a> (Debian-based users will have to add a repository and install the latest version: add-apt-repository ppa:mc3man/mpv-tests -y)</li>
<li><a href="https://globedrill.com/bash-less-command-not-found-install-less-command-on-centos-ubuntu/">less</a> (most distros will already have less installed)</li>
<li><a href="https://github.com/blackjack4494/yt-dlc">youtube-dlc</a></li>
</ul>

<span id="controls"></span>

## Controls

<table>
    <tr>
        <th>Action</th>
        <th>Key</th>
    </tr>
    <tr>
        <td>Pause</td>
        <td>Space</td>
    </tr>
    <tr>
        <td>Mute</td>
        <td>m</td>
    </tr>
    <tr>
        <td>Skip 10 Seconds</td>
        <td>Right Arrow</td>
    </tr>
    <tr>
        <td>Rewind 10 Seconds</td>
        <td>Left Arrow</td>
    </tr>
    <tr>
        <td>Skip 1 Minute</td>
        <td>Up Arrow</td>
    </tr>
    <tr>
        <td>Rewind 1 Minute</td>
        <td>Down Arrow</td>
    </tr>
    <tr>
        <td>Next Song</td>
        <td>></td>
    </tr>
    <tr>
        <td>Previous Song</td>
        <td><</td>
    </tr>
    <tr>
        <td>Stop</td>
        <td>q</td>
    </tr>
</table>

<span id="features"></span>

## Features

<span id="playingPlaylists"></span>

### Playing Playlists

You can play playlists from either on YouTube or your current machine (locally).

<span id="addingPlaylists"></span>

### Adding Playlists

You can add both playlistst from YouTube and from your machine. To add a playlist from YouTube, just copy the link to the playlist and when you have to insert the playlist link in th "Add Playlist" option, just paste the link. When you want to add a playlist from your machine (locally), create a folder, put all the music in there that you want in the playlist, navigate your terminal to that folder (cd /path/to/folder), when inside the folder type the command "pwd" and copy the output. When you have to add the location of the file in "Add Playlist", just paste the copied output into the input. Done.

<span id="editingPlaylists"></span>

### Editing Playlists

You can edit your playlist details, such as the playlist name and the playlist location/link.

<span id="deletingPlaylists"></span>

### Deleting Playlists

Once you have listened too much to a playlist, it is time to delete it. You can easily do that by choosing the delete option.

<span id="shufflingPlaylists"></span>

### Shuffling Playlists

If playing the playlist in it's normal order (YouTube: The way you ordered the playlist | Local: A-Z) then you can shuffle it and enjoy yourself.

<span id="flags"></span>

## Flags

If you don't want to use the interface, you can use flags to do some of the things you need to do. There are 3 types of flags, single-answer flags, multi-answer flags and the playlist flag.

<span id="thePlaylistFlag"></span>

### The playlist flag

This flag is used to run a playlist without opening the interface, this can be done by running bgmplayer with the playlist number afterwards. This should always be added when using multi-answer flags.
Example of the playlist flag: <code>bgmplayer 2</code> (will play playlist nr. 2)

<span id="singleAnswerFlags"></span>

### Single-answer flags

Single-answer flags are flags that does not allow other flags to be added, as an example, the "--help" flag is single-answer and does not allow any other flags to be added before or after it. These flags does 1 thing that does not include working with playlists.
Example of a single answer flag: <code>bgmplayer --help</code>

#### All single-answer flags

<table>
    <tr>
        <th>Flag</th>
        <th>Action</th>
    </tr>
    <tr>
        <td>--help</td>
        <td>Displays help</td>
    </tr>
</table>

<span id="multiAnswerFlags"></span>

### Multi-answer flags

Multi-answer flags are flags that can have multiple flags before or after it, such as the -no-shuffle flag. By default, the playlist you choose shuffles and if you don't want it to shuffle you can use this flag remove the shuffle.
Example of a multi-answer flag: <code>bgmplayer 1 -no-shuffle -some-other-flags</code>

#### All multi-answer flags

<table>
    <tr>
        <th>Flag</th>
        <th>Action</th>
    </tr>
    <tr>
        <td>-no-shuffle</td>
        <td>Disables playlist shuffling</td>
    </tr>
</table>
