import std/[
    strformat, strutils, json
]
from std/sequtils import map, filter
from std/os import execShellCmd, normalizedPath, joinPath
from std/random import shuffle

import utils

# these characters are just effort to maintain, rather not allow them
const badChars = {'*', '\\', '"', '\''}

type
    # this is for updating/editing playlists
    ModType = enum NAME, LINK, DELETE
    SaveFileData = object
        name*: string
        link*: string
    SaveFile = seq[SaveFileData]

# get playlists from playlist file
proc getPlaylists*(): SaveFile =
    try:
        # read the save file, convert it to JSON and structure it like the SaveFile object
        return parseJSON(readFile(utils.appSaveFile)).to(SaveFile)
    except IOError:
        utils.criticalError(&"Failed to open file: {utils.appSaveFile}")
    except JsonParsingError:
        utils.criticalError(&"Could not read json, please check config file: {utils.appSaveFile}")

# This function will check if a playlist already exists in the playlist files
proc checkPlaylistExists(playlistName: string): bool =
    if contains(playlistName, badChars) or (len(playlistName.strip()) < 1):
        utils.showMessage("Invalid name entered", utils.MessageType.WARN)
        return true

    let foundFiles = filter(getPlaylists(), proc(item: SaveFileData): bool = item.name.strip() == playlistName.strip())

    if len(foundFiles) > 0:
        utils.showMessage("Playlist with same name already exists", utils.MessageType.WARN)
        return true

    return false

proc checkPlaylistFileEmpty(): bool =
    try:
        return len(parseJSON(readFile(utils.appSaveFile))) < 1
    except IOError:
        utils.criticalError(&"Failed to read file: {utils.appSaveFile}")
    except JsonParsingError:
        # if it cannot parse the json, good chance it is empty
        return true

proc rewritePlaylistsFile(playlistNumber: int, modType: ModType) =
    var playlists: SaveFile = getPlaylists()

    if modType == ModType.DELETE:
        playlists.del(playlistNumber)
    else:
        # get the new value
        stdout.write("Enter new value (cancel to cancel): ")
        let change: string = readLine(stdin).strip()

        # verify user input
        if change == "cancel" or change == "0" or change == "" or playlistNumber == 0:
            utils.showMessage("--- Operation Canceled ---", utils.MessageType.NOTICE)
            return

        # user should never get to here, but just in case, we add an else
        case modType:
            of ModType.NAME:
                if checkPlaylistExists(change):
                    rewritePlaylistsFile(playlistNumber, modType)
                    return

                playlists[playlistNumber].name = change
            of ModType.LINK:
                playlists[playlistNumber].link = change
            else:
                utils.criticalError("Unknown value given to playlist file rewrite!")

    try:
        # rewrite the save file
        let saveFile = open(utils.appSaveFile, fmWrite)
        # we use pretty incase the user wants to manually edit the playlist file
        saveFile.write(pretty(%playlists))
        saveFile.close()

        utils.showMessage("--- Playlists Updated ---", utils.MessageType.SUCCESS)
    except IOError:
        utils.showMessage(&"Failed to write to file: {utils.appSaveFile}", utils.MessageType.WARN)

proc displayPlayerControls() =
    echo "\nPlayer Controls:"
    echo "\t[SPACE] - Pause\t\t\tm - Mute"
    echo "\t[Right Arrow] - Skip 10 Seconds\t[Left Arrow] - Rewind 10 Seconds"
    echo "\t[Up Arrow] - Skip 1 Minute\t[Down Arrow] - Rewind 1 Minute"
    echo "\t> - Next Song\t\t\t< - Previous Song"
    echo "\tq - Quit"

# if the user wants to play playlists without opening the interface
proc instantPlayPlaylists*(playlistsToPlay: openArray[int], shuffle: bool = false, loop: bool = false,
        random: bool = false) =
    if(checkPlaylistFileEmpty()):
        utils.showMessage("No playlists have been added, playlist file is empty.", utils.MessageType.WARN)
        return

    var
        playlists: SaveFile = getPlaylists()
        # start of the mpv command to execute
        command = "mpv"

    if random:
        # if the user selected random, it will add all their playlists
        # shuffle all the playlists
        shuffle(playlists)
        # add the links to all the playlists that has to be played
        for index, playlist in playlists:
            command &= " " & utils.cleanFilePath(playlist.link)
    else:
        # add the links to all the playlists that has to be played
        for _, p in playlistsToPlay:
            let originalPlaylistNr = p + 1
            # playlist number are determined by the line number it can be found on, so
            # below makes sure that the line number actually exists, if it does not
            # it will throw an error
            if len(playlists) < originalPlaylistNr:
                utils.criticalError(&"Playlist nr {originalPlaylistNr} does not exist.")

            command &= " " & utils.cleanFilePath(playlists[p].link)

    # remove video, so only music plays
    command &= &" --no-video {utils.scriptOpts}"

    if shuffle or random:
        command &= " --shuffle"
        utils.showMessage("Shuffling playlists", utils.MessageType.NOTICE)

    if loop or random:
        command &= " --loop-playlist"
        utils.showMessage("Looping playlists", utils.MessageType.NOTICE)

    displayPlayerControls()

    # todo this command (mpv) will only work on Linux (and maybe Mac)
    if execShellCmd(command) != 0:
        # this will save the program, in the future we can consider
        # not stopping it from running and allow the user to choose
        # a playlist again
        utils.criticalError("Error while trying to run MPV command")

proc playPlaylists*(ctx: auto) =
    if(checkPlaylistFileEmpty()):
        utils.showMessage("No playlists have been added, playlist file is empty.", utils.MessageType.WARN)
        return

    let playlists: SaveFile = getPlaylists();

    for index, playlist in playlists:
        echo &"{index + 1}. {playlist.name}"

    echo "0. Cancel"

    stdout.write("(Playlists to play [separated by spaces]) > ")

    let chosenPlaylists: seq[string] = readLine(stdin).strip().split(" ")
    var cleanedChosenPlaylists: seq[int] = @[]

    if len(chosenPlaylists) < 1:
        echo "Please enter at least 1 number"
        playPlaylists(ctx)
        return

    for index, choice in chosenPlaylists:
        if choice == "0":
            utils.showMessage("--- Operation Canceled ---", utils.MessageType.NOTICE)
            return

        try:
            # try to convert input to numbers
            if parseInt(choice) < 1:
                utils.showMessage("Invalid option detected. Please only enter valid numbers.", utils.MessageType.WARN)
                playPlaylists(ctx)
                return

            cleanedChosenPlaylists.add(parseInt(choice) - 1)
        except ValueError:
            utils.showMessage("Invalid option detected. Please only enter numbers.", utils.MessageType.WARN)
            playPlaylists(ctx)
            return

    var command = ""
    # var command = "mpv"

    for index, playlist in playlists:
        if cleanedChosenPlaylists.find(index) < 0:
            continue

        if len(command) < 1:
            command &= utils.cleanFilePath(playlist.link)
        else:
            command &= " " & utils.cleanFilePath(playlist.link)

    # command &= &" --no-video {utils.scriptOpts}"

    if utils.getYesNoAnswer("Would you like to shuffle the playlists?"):
        # command &= " --shuffle"
        ctx.set_option("shuffle", "yes")

    if utils.getYesNoAnswer("Loop playlists?"):
        # command &= " --loop-playlist"
        ctx.set_option("loop-playlist", "yes")

    displayPlayerControls()

    ctx.command("loadfile", command)

    # todo this command (mpv) will only work on Linux (and maybe Mac)
    # if execShellCmd(command) != 0:
        # this will save the program, in the future we can consider
        # not stopping it from running and allow the user to choose
        # a playlist again
        # utils.criticalError("Error while trying to run MPV command")

proc addPlaylist*() =
    stdout.write("Please enter a name for the new playlist(type cancel to cancel): ")

    let playlistName: string = readLine(stdin)

    if playlistName == "cancel" or playlistName == "0":
        utils.showMessage("--- Operation Canceled ---", utils.MessageType.NOTICE)
        return

    if checkPlaylistExists(playlistName):
        addPlaylist()
        return

    utils.showMessage("NOTE: If you're adding a local playlist, the path should look similar to this: /home/username/my music",
            utils.MessageType.NOTICE)
    # todo this is not windows safe, will only work on linux
    utils.showMessage("NOTE: Please do not add any '\\', '*', '~', '`', '\"' or '\'' to the location and start from the root directory if on Linux",
            utils.MessageType.NOTICE)

    stdout.write("Please enter the location of the folder or the link to the playlist (cancel to cancel): ")

    let playlistLocation: string = readLine(stdin)
    if len(playlistLocation) < 1:
        utils.showMessage("Location is invalid", utils.MessageType.WARN)
        addPlaylist()
        return

    if playlistLocation == "cancel":
        utils.showMessage("--- Operation Canceled ---", utils.MessageType.NOTICE)
        return

    # I think we limit it because of MPV, couldn't remember
    # so if the playlist is saved in a local directory and the length
    # is > 60 characters
    if playlistLocation[0] == '/' and (len(playlistLocation) > 60):
        utils.showMessage("Path to playlist is too long. Try moving it to a folder closer to root (/)",
                utils.MessageType.WARN)
        addPlaylist()
        return

    try:
        var saveFileContent: SaveFile = parseJSON(readFile(utils.appSaveFile)).to(SaveFile)

        saveFileContent.add(SaveFileData(name: playlistName, link: playlistLocation))

        let saveFile = open(utils.appSaveFile, fmWrite)
        saveFile.write(pretty(%saveFileContent))
        saveFile.close()
    except IOError:
        utils.showMessage(&"Failed to write to file: {utils.appSaveFile}", utils.MessageType.WARN)

proc updatePlaylist*() =
    const playlistModifyOptions: array[2, string] = ["Playlist Name", "Playlist Link/Location"]

    if checkPlaylistFileEmpty():
        utils.showMessage("No playlists have been added, so there are no playlists to update...",
                utils.MessageType.WARN)
        return

    let playlists: SaveFile = getPlaylists();
    let playlistNames: seq[string] = map(playlists, proc(val: SaveFileData): string = $val.name)

    let chosenPlaylist: int = utils.getSelectableOption("Choose a playlist:", playlistNames, "(Playlist to edit) > ")

    if chosenPlaylist == 0:
        return

    let chosenOption: int = utils.getSelectableOption("What would you like to modify?", playlistModifyOptions, "(What to edit) > ")

    case chosenOption:
        of 0:
            utils.showMessage("Playlist not updated", utils.MessageType.WARN)
            return
        of 1:
            # -1 since we use 0 as cancel and indexes start at 0
            rewritePlaylistsFile(chosenPlaylist-1, ModType.NAME)
        of 2:
            rewritePlaylistsFile(chosenPlaylist-1, ModType.LINK);
        else:
            # user should never end up here, but if they somehow do
            # just start over
            utils.showMessage("Invalid option", utils.MessageType.WARN)
            updatePlaylist()
            return

proc removePlaylist*() =
    if checkPlaylistFileEmpty():
        utils.showMessage("No playlists have been added, so there are no playlists to remove...",
                utils.MessageType.NOTICE)
        return

    let playlists: SaveFile = getPlaylists()

    let playlistNames: seq[string] = map(playlists, proc(val: SaveFileData): string = $val.name)

    let chosenPlaylist: int = utils.getSelectableOption("Choose a playlist:", playlistNames, "(Playlist to delete) > ")

    if chosenPlaylist == 0:
        utils.showMessage("--- Operation Canceled ---", utils.MessageType.NOTICE)
        return

    if not utils.getYesNoAnswer(&"Are you sure you want to delete playlist \"{playlistNames[chosenPlaylist-1]}\" forever?"):
        utils.showMessage("Playlist NOT deleted", utils.MessageType.NOTICE)
        return

    rewritePlaylistsFile(chosenPlaylist-1, ModType.DELETE)
