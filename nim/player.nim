from os import getConfigDir, joinPath, existsOrCreateDir, fileExists
import strformat
import utils
from strutils import strip, split, find

const 
    scriptOpts: string = "--script-opts=ytdl_hook-ytdl_path=/usr/local/bin/yt-dlp"
    saveFolderName: string = "ytbgmpcli"
    saveFileName: string = "playlists.txt"

let appSaveFile: string = joinPath(getConfigDir(), saveFolderName, saveFileName)

proc setup*(): bool =
    let configDir = joinPath(getConfigDir(), saveFolderName)

    try:
        # check if dir exists, if not, create it
        if not existsOrCreateDir(configDir):
            echo "Config folder not found, creating it..."
    except OSError:
        echo &"Failed to create config directory {configDir}"
        return false

    try:
        if not fileExists(appSaveFile):
            echo "Save file not found, creating it..."
            writeFile(appSaveFile, "")
    except OSError:
        echo &"Failed to create config file: {appSaveFile}"
        return false

    return true

proc checkPlaylistExists(playlistName: string): bool =
    if find(playlistName, "`") != -1 or find(playlistName, "~") != -1:
        utils.showMessage("Invalid character found '~' or '`' in name", "warning")
        return true
    
    var playlistFile: File

    if not open(playlistFile, appSaveFile): # check if file can be opened
        utils.criticalError(&"Failed to open file: {appSaveFile}")

    var 
        readingFile: bool = true
        readingLine: int = 0

    while readingFile:
        readingLine += 1
        try:
            # if online, then we use ~, if locally, then we use `
            # this is legacy code, might be changed in the future
            # to use "/" and "http" instead of "~" and "`", since
            # local playlists will always start with / and online with http(s)
            var line: seq[string] = playlistFile.readLine().strip().split("~")

            if line.len != 2:
                # if the playlist is stored locally
                line = playlistFile.readLine().strip().split("`")

                if line.len != 2:
                    readingFile = false
                    utils.criticalError(&"Invalid line ({readingLine}) in file: {appSaveFile}")

            let playlist: string = line[0]

            if playlist == playlistName:
                utils.showMessage("Playlist with same name already exists", "warning")
                close(playlistFile)
                return true
        except EOFError:
            readingFile = false
        except:
            readingFile = false
            close(playlistFile)
            utils.criticalError(&"Failed to read file: {appSaveFile}")

    close(playlistFile)
    
    return false

proc addPlaylist*() =
    stdout.write("Please enter a name for the new playlist(type cancel to cancel): ")

    let playlistName: string = readLine(stdin)

    if playlistName == "cancel":
        echo "--- Operation Canceled ---"
        return

    if checkPlaylistExists(playlistName):
        addPlaylist()
        return

    utils.showMessage("NOTE: If you're adding a local playlist, this is how your location should look: /home/netsu/my music", "notice")
    # todo this is not windows safe, will only work on linux
    utils.showMessage("NOTE: Please do not add any '\\', '\"' or '\'' to the location and start from the root directory if on Linux", "notice")

    stdout.write("Please enter the location of the folder or the link to the playlist (cancel to cancel): ")

    let playlistLocation: string = readLine(stdin)
    if playlistLocation.len < 1:
        utils.showMessage("Location is invalid", "warning")
        addPlaylist()
        return

    if playlistLocation == "cancel":
        echo "--- Operation Canceled ---"
        return

    let localPlaylist: bool = playlistLocation[0] == '/'
    
    if localPlaylist and (playlistLocation.len > 60):
        utils.showMessage("Path to playlist is too long. Try moving it to a folder closer to root (/)", "warning")
        addPlaylist()
        return

    try:
        let saveFile = open(appSaveFile, fmAppend)

        if localPlaylist:
            saveFile.write(&"\n{playlistName}`{playlistLocation}")
        else:
            saveFile.write(&"\n{playlistName}~{playlistLocation}")

        saveFile.close()
    except IOError:
        utils.showMessage(&"Failed to write to file: {appSaveFile}", "warning")
    except:
        utils.showMessage(&"Failed to open file: {appSaveFile}", "warning")