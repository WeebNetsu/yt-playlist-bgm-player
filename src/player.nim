import strformat
from strutils import strip, split, find, parseInt, toLowerAscii, replace
from sequtils import map
from os import execShellCmd, normalizedPath, joinPath

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
    try:
        let playlistFile: string = readFile(utils.appSaveFile)

        if len(playlistFile.strip()) < 3:
            return true
    except:
        utils.criticalError(&"Failed to read file: {utils.appSaveFile}")

    return false

proc getPlaylists*(): seq[tuple[name: string, location: string, local: bool]] =
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

proc updatePlaylistLink(playlistNumber: int) =
    stdout.write("New link/location for playlist (cancel to cancel): ")
    let newLocation: string = readLine(stdin).strip()

    if newLocation == "cancel":
        utils.showMessage("--- Operation Canceled ---", "notice")
        return

    if newLocation == "":
        utils.showMessage("Invalid link/location provided", "warning")
        updatePlaylistLink(playlistNumber)
        return

    if rewritePlaylistsFile(playlistNumber, ModType.LINK, newLocation):
        utils.showMessage("--- Playlist Updated ---", "success")
    else:
        utils.showMessage("Playlist could not be updated", "warning")
  
proc displayPlayerControls() =
    echo "\nPlayer Controls:"
    echo "\t[SPACE] - Pause\t\t\tm - Mute"
    echo "\t[Right Arrow] - Skip 10 Seconds\t[Left Arrow] - Rewind 10 Seconds"
    echo "\t[Up Arrow] - Skip 1 Minute\t[Down Arrow] - Rewind 1 Minute"
    echo "\t> - Next Song\t\t\t< - Previous Song"
    echo "\tq - Quit"
   
# if the user wants to play playlists without opening the interface
proc instantPlayPlaylists*(playlistsToPlay: openArray[int], shuffle: bool = false, loop: bool = false) =
    if(checkPlaylistFileEmpty()):
        utils.showMessage("No playlists have been added, playlist file is empty.", "warning")
        return

    let playlists: seq[tuple[name: string, location: string, local: bool]] = getPlaylists();

    var command = "mpv"

    for index, playlist in playlists:
        if playlistsToPlay.find(index) < 0:
            continue

        command &= " " & utils.cleanFilePath(playlist.location)

    command &= &" --no-video {utils.scriptOpts}"

    if shuffle:
        command &= " --shuffle"

    if loop:
        command &= " --loop-playlist"
    
    displayPlayerControls()

    # todo this command (mpv) will only work on Linux (and maybe Mac)
    if execShellCmd(command) != 0:
        # this will save the program, in the future we can consider
        # not stopping it from running and allow the user to choose
        # a playlist again
        utils.criticalError("Error while trying to run MPV command")
    
proc playPlaylists*() =
    if(checkPlaylistFileEmpty()):
        utils.showMessage("No playlists have been added, playlist file is empty.", "warning")
        return

    let playlists: seq[tuple[name: string, location: string, local: bool]] = getPlaylists();
    let playlistNames: seq[string] = map(playlists, proc(val: tuple[name: string, location: string, local: bool]): string = $val.name)

    for index, playlistName in playlistNames:
        echo &"{index + 1}. {playlistName}"

    echo "0. Cancel"

    stdout.write("(Playlists to play [separated by spaces]) > ")

    let chosenPlaylists: seq[string] = readLine(stdin).strip().split(" ")
    var cleanedChosenPlaylists: seq[int] = @[]

    if len(chosenPlaylists) < 1:
        echo "Please enter at least 1 number"
        playPlaylists()
        return

    for index, choice in chosenPlaylists:
        if choice == "0":
            utils.showMessage("--- Operation Canceled ---", "notice")
            return

        try:
            if parseInt(choice) < 1:
                utils.showMessage("Invalid option detected. Please only enter valid numbers.", "warning")
                playPlaylists()
                return

            cleanedChosenPlaylists.add(parseInt(choice) - 1)
        except ValueError:
            utils.showMessage("Invalid option detected. Please only enter numbers.", "warning")
            playPlaylists()
            return

    var command = "mpv"

    for index, playlist in playlists:
        if cleanedChosenPlaylists.find(index) < 0:
            continue

        command &= " " & utils.cleanFilePath(playlist.location)

    command &= &" --no-video {utils.scriptOpts}"

    if utils.getYesNoAnswer("Would you like to shuffle the playlists?"):
        command &= " --shuffle"

    if utils.getYesNoAnswer("Loop playlists?"):
        command &= " --loop-playlist"
    
    displayPlayerControls()

    # todo this command (mpv) will only work on Linux (and maybe Mac)
    if execShellCmd(command) != 0:
        # this will save the program, in the future we can consider
        # not stopping it from running and allow the user to choose
        # a playlist again
        utils.criticalError("Error while trying to run MPV command")

proc addPlaylist*() =
    stdout.write("Please enter a name for the new playlist(type cancel to cancel): ")

    let playlistName: string = readLine(stdin)

    if playlistName == "cancel":
        utils.showMessage("--- Operation Canceled ---", "notice")
        return

    if checkPlaylistExists(playlistName):
        addPlaylist()
        return

    utils.showMessage("NOTE: If you're adding a local playlist, this is how your location should look: /home/netsu/my music", "notice")
    # todo this is not windows safe, will only work on linux
    utils.showMessage("NOTE: Please do not add any '\\', '*', '~', '`', '\"' or '\'' to the location and start from the root directory if on Linux", "notice")

    stdout.write("Please enter the location of the folder or the link to the playlist (cancel to cancel): ")

    let playlistLocation: string = readLine(stdin)
    if len(playlistLocation) < 1:
        utils.showMessage("Location is invalid", "warning")
        addPlaylist()
        return

    if playlistLocation == "cancel":
        utils.showMessage("--- Operation Canceled ---", "notice")
        return

    let localPlaylist: bool = playlistLocation[0] == '/'
    
    # I think we limit it because of MPV, couldn't remember
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

    let chosenOption: int = utils.getSelectableOption("What would you like to modify?", playlistModifyOptions, "(What to edit) > ")

    case chosenOption:
        of 0:
            utils.showMessage("Playlist not updated", "warning")
            return
        of 1:
            # -1 since we use 0 as cancel and indexes start at 0
            updatePlaylistName(chosenPlaylist-1)
        of 2:
            updatePlaylistLink(chosenPlaylist-1);
        of 3:
            # change playlist status from online -> local and vice versa
            if rewritePlaylistsFile(chosenPlaylist-1, ModType.LOCAL):
                utils.showMessage("--- Playlist Updated ---", "success")
            else:
                utils.showMessage("Playlist could not be updated", "warning")
        else:
            # user should never end up here, but if they somehow do
            # just start over
            utils.showMessage("Invalid option", "warning")
            updatePlaylist()
            return

proc removePlaylist*() =
    if checkPlaylistFileEmpty():
        utils.showMessage("No playlists have been added, so there are no playlists to remove...", "notice")
        return

    let playlists: seq[tuple[name: string, location: string, local: bool]] = getPlaylists()

    let playlistNames: seq[string] = map(playlists, proc(val: tuple[name: string, location: string, local: bool]): string = $val.name)

    let chosenPlaylist: int = utils.getSelectableOption("Choose a playlist:", playlistNames, "(Playlist to delete) > ")

    if chosenPlaylist == 0:
        utils.showMessage("--- Operation Canceled ---", "notice")
        return

    if not utils.getYesNoAnswer(&"Are you sure you want to delete playlist \"{playlistNames[chosenPlaylist-1]}\" forever?"):
        utils.showMessage("Playlist NOT deleted", "notice")
        return
    
    if rewritePlaylistsFile(chosenPlaylist-1, ModType.DELETE):
        utils.showMessage("Playlist deleted", "notice")
