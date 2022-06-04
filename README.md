# YouTube Playlist BGM Player

This is a basic background music player made with Nim to play YouTube Playlists in the background. It is focused on being very light weight and easily accessable, even if you don't have a Desktop Enviroment or Window Manager.

Notes:

<ul>
    <!-- it may have Mac support, but I don't have a Mac to test it on -->
    <li>This currently only tested on Linux (it probably works on Mac), I'm planning to give it support for other OSs in the future.</li>
</ul>

## Content

<ul>
    <li><a href="#howToRun">How to install and run</a></li>
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
</ul>

<span id="howToRun"></span>

## How to install and run

_Note: You should have a stable internet connection to use the installer and YouTube playlist player. Also, the installer only works on **Ubuntu** and **Arch** based distros._

_Note: The executable inside the `sh` folder might be outdated (I might forget to move new executable to sh folder)! If you experience problems with it, compile the source code to generate a new executable._

Clone or download the files and make sure the installer.sh file is executable with:<br>
`chmod +x installer.sh`<br>
Run the installer.sh file (Ubuntu and Arch based distros only) and you're ready to run the application! To run the application, execute the following command in your terminal:<br>`bgmplayer`<br>
**If you don't have an Ubuntu or Arch based Linux distro**, you can compile the code (or use the prebuilt "bgmplayer" file) and just mimic everything happening in the installer.sh, but modified for your system. The steps are simple:

1. Compile the code into an executable (optional)
   1.5. If you compiled it yourself, rename the executable to "bgmplayer"
2. Make the executable (prebuilt, or manually compiled) runnable with `chmod +x bgmplayer`
3. Move/Copy the file to your $HOME/bin directory (create it if it does not exist)
4. Install MPV and Python 3
5. Install yt-dlp with `yt-dlp --version || ( sudo wget https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp -O /usr/local/bin/yt-dlp && sudo chmod a+rx /usr/local/bin/yt-dlp )`
6. Add $HOME/bin to PATH (if not there already). (Put code inside .bashrc to avoid having to do it on every boot)
7. Reboot

<span id="howToUpdate"></span>

## How to update

If the application has a new version, just download it and put the executable inside your `$HOME/bin` directory.

<span id="compilingTheCode"></span>

## Compiling the code

If you want to compile the source code yourself, you'll need the dependencies. You can either compile by executing `compile.sh` or `install.sh` (`install.sh` will also install application and dependencies) in the `sh/` directory, or by doing the following inside the `src/` directory: `nim c main.nim`

**NOTE:** `compile.sh` will generate the new executable and also save it inside the `sh` folder.

<span id="dependencies"></span>

### Dependencies

<ul>
<li><a href="https://nim-lang.org" target="_blank">Nim</a> <small>(only if compiling from source)</small></li>
<li><a href="https://github.com/nim-lang/nimble" target="_blank">Nimble</a> <small>(only if compiling from source)</small></li>
<li>The <b>latest</b> version of <a href="https://mpv.io/installation/" target="_blank">mpv</a> (Debian/Ubuntu-based users will have to add a repository and install the latest version: add-apt-repository ppa:mc3man/mpv-tests -y)</li>
<!-- less - because I don't know how to replicate it with C++ yet -->
<li><a href="https://github.com/yt-dlp/yt-dlp" target="_blank">yt-dlp</a></li>
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

This flag is used to run a playlist without opening the interface, this can be done by running bgmplayer with the playlist number afterwards. This should always be added when using multi-answer flags.<br>Example of the playlist flag: <code>bgmplayer 2</code> (will play playlist nr. 2)<br>Another example of the playlist flag: <code>bgmplayer 1 2</code> (will play playlist nr. 1 & 2)

### All other flags

<table>
    <tr>
        <th>Flag</th>
        <th>Short</th>
        <th>Action</th>
    </tr>
    <tr>
        <td>--help</td>
          <td>-h</td>
        <td>Displays help</td>
    </tr>
    <tr>
        <td>--list</td>
          <td>-l</td>
        <td>Displays all playlists</td>
    </tr>
    <tr>
        <td>--random</td>
          <td>-r</td>
        <td>Play all your playlists in random order</td>
    </tr>
    <tr>
        <td>--no-shuffle</td>
        <td>-n</td>
        <td>Disables playlist shuffling</td>
    </tr>
    <tr>
        <td>--no-loop</td>
        <td>N/A</td>
        <td>Disables playlist looping</td>
    </tr>
</table>
