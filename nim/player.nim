import strformat
from strutils import strip, split, find, parseInt
from sequtils import map

import utils

# this is for updating/editing playlists
type ModType = enum NAME, LINK, LOCAL, DELETE

proc checkPlaylistExists(playlistName: string): bool =
    if find(playlistName, "`") != -1 or find(playlistName, "~") != -1:
        utils.showMessage("Invalid character found '~' or '`' in name", "warning")
        return true

    if(len(playlistName) < 1):
        utils.showMessage("Invalid name entered", "warning")
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

            #  don't read empty lines
            if lineRead == "":
                continue

            # if online, then we use ~, if locally, then we use `
            # this is legacy code, might be changed in the future
            # to use "/" and "http" instead of "~" and "`", since
            # local playlists will always start with / and online with http(s)
            var line: seq[string] = lineRead.split("~")

            if len(line) != 2:
                # if the playlist is stored locally
                line = lineRead.split("`")

                if len(line) != 2:
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

proc checkPlaylistFileEmpty(): bool =
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

proc getPlaylists(): seq[tuple[name: string, location: string, local: bool]] =
    result = @[]
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

            #  don't read empty lines
            if lineRead == "":
                continue

            var line: seq[string] = lineRead.split("~")
            var local: bool = false

            if len(line) != 2:
                # if the playlist is stored locally
                line = lineRead.split("`")
                local = true

                if len(line) != 2:
                    readingFile = false
                    utils.criticalError(&"Invalid line ({readingLine}) in file: {utils.appSaveFile}")

            # line @ index 0 = playlist name
            # line @ index 1 = playlist link/path
            # echo &"{readingLine}. {line[0]}"
            result.add((line[0], line[1], local))
        except EOFError:
            readingFile = false
        except:
            readingFile = false
            close(playlistFile)
            utils.criticalError(&"Failed to read file: {utils.appSaveFile}")

    close(playlistFile)

proc rewritePlaylistsFile(playlistNumber: int, modType: ModType, newChange: string = ""): bool = 
    var playlists: seq[tuple[name: string, location: string, local: bool]] = getPlaylists()

    case modType:
        of ModType.NAME:
            # since 0 is used for canceling, user input starts at 1
            playlists[playlistNumber].name = newChange
        of ModType.LINK:
            playlists[playlistNumber].location = newChange
        of ModType.LOCAL:
            playlists[playlistNumber].local = not playlists[playlistNumber].local
        of ModType.DELETE:
            playlists.del(playlistNumber)

    var newList: string = ""
    for playlist in playlists:
        if playlist.local:
            newList &= &"\n{playlist.name}`{playlist.location}"
        else:
            newList &= &"\n{playlist.name}~{playlist.location}"

    try:
        let saveFile = open(utils.appSaveFile, fmWrite)
        saveFile.write(newList)
        saveFile.close()

        return true
    except IOError:
        utils.showMessage(&"Failed to write to file: {utils.appSaveFile}", "warning")
    except:
        utils.showMessage(&"Failed to open file: {utils.appSaveFile}", "warning")

    return false

proc updatePlaylistName(playlistNumber: int) =
    stdout.write("New name for playlist (cancel to cancel): ")
    let newName: string = readLine(stdin).strip()

    if newName == "cancel":
        utils.showMessage("--- Operation Canceled ---", "notice")
        return

    if checkPlaylistExists(newName):
        updatePlaylistName(playlistNumber)
        return

    if rewritePlaylistsFile(playlistNumber, ModType.NAME, newName):
        utils.showMessage("--- Playlist Updated ---", "success")
    else:
        utils.showMessage("Playlist could not be updated", "warning")
    

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
    if len(playlistLocation) < 1:
        utils.showMessage("Location is invalid", "warning")
        addPlaylist()
        return

    if playlistLocation == "cancel":
        echo "--- Operation Canceled ---"
        return

    let localPlaylist: bool = playlistLocation[0] == '/'
    
    if localPlaylist and (len(playlistLocation) > 60):
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
    const playlistModifyOptions: array[3, string] = ["Playlist Name", "Playlist Link/Location", "Local/Online"]

    if checkPlaylistFileEmpty():
        utils.showMessage("No playlists have been added, so there are no playlists to update...", "warning")
        return

    let playlists: seq[tuple[name: string, location: string, local: bool]] = getPlaylists();
    let playlistNames: seq[string] = map(playlists, proc(val: tuple[name: string, location: string, local: bool]): string = $val.name)

    let chosenPlaylist: int = utils.getSelectableOption("Choose a playlist:", playlistNames, "(Playlist to edit) > ")

    if chosenPlaylist == 0:
        return

    if (chosenPlaylist > len(playlists)):
        utils.showMessage("Could not find playlist... Does it exist?", "warning")
        updatePlaylist()
        return

    let chosenOption: int = utils.getSelectableOption("What would you like to modify?", playlistModifyOptions, "(What to edit) > ")

    case chosenOption:
        of 0:
            utils.showMessage("Playlist not updated", "warning")
            return
        of 1:
            # -1 since we use 0 as cancel and indexes start at 0
            updatePlaylistName(chosenPlaylist-1)
        of 2:
            utils.showMessage("Update Link", "notice")
            # updatePlaylistLink(chosenPlaylist);
        of 3:
            # change playlist status from online -> local and vice versa
            # rewriteFile(rewritePlaylists(chosenPlaylist, LOCAL, ""));
            utils.showMessage("Update type", "notice")
        else:
            # user should never end up here, but if they somehow do
            # just start over
            utils.showMessage("Invalid option", "warning")
            updatePlaylist()
            return

