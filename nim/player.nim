import strformat
from strutils import strip, split, find

import utils


proc checkPlaylistExists*(playlistName: string): bool =
    if find(playlistName, "`") != -1 or find(playlistName, "~") != -1:
        utils.showMessage("Invalid character found '~' or '`' in name", "warning")
        return true
    
    var playlistFile: File

    if not open(playlistFile, utils.appSaveFile): # check if file can be opened
        utils.criticalError(&"Failed to open file: {utils.appSaveFile}")

    var 
        readingFile: bool = true
        readingLine: int = 0

    while readingFile:
        readingLine += 1
        try:
            let lineRead = playlistFile.readLine().strip()
            # if online, then we use ~, if locally, then we use `
            # this is legacy code, might be changed in the future
            # to use "/" and "http" instead of "~" and "`", since
            # local playlists will always start with / and online with http(s)
            var line: seq[string] = lineRead.split("~")

            if line.len != 2:
                # if the playlist is stored locally
                line = lineRead.split("`")

                if line.len != 2:
                    readingFile = false
                    utils.criticalError(&"Invalid line ({readingLine}) in file: {utils.appSaveFile}")

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
            utils.criticalError(&"Failed to read file: {utils.appSaveFile}")

    close(playlistFile)
    
    return false

proc checkPlaylistFileEmpty*(): bool =
    var playlistFile: File
    if not open(playlistFile, utils.appSaveFile): # check if file can be opened
        utils.criticalError(&"Failed to open file: {utils.appSaveFile}")
        
    try:
        discard playlistFile.readLine()
    except EOFError:
        # if we get end of file on first read, then the file is empty
        close(playlistFile)
        return true
    except:
        utils.criticalError(&"Failed to read file: {utils.appSaveFile}")

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
        let saveFile = open(utils.appSaveFile, fmAppend)

        if localPlaylist:
            saveFile.write(&"\n{playlistName}`{playlistLocation}")
        else:
            saveFile.write(&"\n{playlistName}~{playlistLocation}")

        saveFile.close()
    except IOError:
        utils.showMessage(&"Failed to write to file: {utils.appSaveFile}", "warning")
    except:
        utils.showMessage(&"Failed to open file: {utils.appSaveFile}", "warning")

proc updatePlaylist*() =
    if checkPlaylistFileEmpty():
        utils.showMessage("No playlists have been added, so there are no playlists to update...", "warning")
        return

    echo "Playlists you have:"
