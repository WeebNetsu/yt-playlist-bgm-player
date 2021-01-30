# YouTube Playlist BGM Player

This is a basic background music player made with C++ to play YouTube Playlists in the background. It is focused on being very light weight and easily accessable, even if you don't have a Desktop Enviroment or Window Manager (so cli only).
Note: this only works on Linux, I'm planning to give it support for other OSs in the future.

## How to run

<i>Note: You should have a stable internet connection to use the installer and YouTube playlist player.</i>

Clone or download the files and make sure the debian_based_installer.sh file is executable with:<br>
<code><pre>chmod +x debian_based_installer.sh</pre></code>
Run the debian_based_installer.sh file (debian only) and you're ready to run the application! To run the application, execute the following command in your terminal:
<code><pre>bgmplayer</pre></code>

If you don't have a Debian based linux, you can compile the code (or use the existing output file) and just mimic everything happening in the debian_based_installer.sh, but modified for your system (so not apt, but pacman or whatever package manager you use).

## How to update

If the application has a new version, you can redownload the source code and run the debian_based_installer.sh file. It will update whatever should be updated. If you don't have a Debian based system, just move the output file to a bin folder or add it to path for the latest version (you can compile it yourself if you want)

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

## Features

### Playing Playlists

You can play playlists from either on YouTube or your current machine (locally).

### Adding Playlists

You can add both playlistst from YouTube and from your machine. To add a playlist from YouTube, just copy the link to the playlist and when you have to insert the playlist link in th "Add Playlist" option, just paste the link. When you want to add a playlist from your machine (locally), create a folder, put all the music in there that you want in the playlist, navigate your terminal to that folder (cd /path/to/folder), when inside the folder type the command "pwd" and copy the output. When you have to add the location of the file in "Add Playlist", just paste the copied output into the input. Done.

### Editing Playlists

You can edit your playlist details, such as the playlist name and the playlist location/link.

### Deleting Playlists

Once you have listened too much to a playlist, it is time to delete it. You can easily do that by choosing the delete option.

### Shuffling Playlists

If playing the playlist in it's normal order (YouTube: The way you ordered the playlist | Local: A-Z) then you can shuffle it and enjoy yourself.
