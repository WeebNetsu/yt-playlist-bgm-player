import strformat
from os import getConfigDir, joinPath, existsOrCreateDir, fileExists
from strutils import parseInt, toLowerAscii, strip
from terminal import setForegroundColor, resetAttributes, ForegroundColor

const 
    scriptOpts*: string = "--script-opts=ytdl_hook-ytdl_path=/usr/local/bin/yt-dlp"
    saveFolderName*: string = "ytbgmpcli"
    saveFileName*: string = "playlists.txt"

let appSaveFile*: string = joinPath(getConfigDir(), saveFolderName, saveFileName)

proc displayMenu*(menuOptions: openArray[string]) =
    echo "What would you like to do?"

    for index, option in menuOptions:
        echo &"{index + 1}. {option}"

    echo "0. Exit"

proc criticalError*(errorMessage: string) =
    # todo display message in red
    setForegroundColor(ForegroundColor.fgRed)
    echo &"Critical Error: {errorMessage}"
    stdout.resetAttributes() # reset terminal colors & stuff
    system.quit()

proc showMessage*(msg: string, msgType: string = "success") =
    case msgType:
        of "notice":
            setForegroundColor(ForegroundColor.fgBlue)
            echo msg
        of "warning":
            setForegroundColor(ForegroundColor.fgYellow)
            # only warning and errors will prepended to the message
            echo "Warning: ", msg
        of "success":
            setForegroundColor(ForegroundColor.fgGreen)
            echo msg

    stdout.resetAttributes() # reset terminal colors & stuff

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

proc getSelectableOption*(question: string, options: openArray[string], inputStr: string = "> "): int =
    echo question

    for index, option in options:
        echo &"{index + 1}. {option}"

    echo "0. Cancel"
    stdout.write(inputStr)

    try:
        let chosenOption: int = readLine(stdin).parseInt()

        if chosenOption > len(options):
            showMessage("Invalid option... Does it exist?", "warning")
            return getSelectableOption(question, options, inputStr)

        return chosenOption
    except ValueError:
        echo "Invalid choice"
        return getSelectableOption(question, options, inputStr)
    except:
        utils.criticalError(&"Unknown error while getting user input.")

proc getYesNoAnswer*(question: string): bool =
    stdout.write(question & " [y/n]: ")

    let confirm: string = readLine(stdin).strip()

    if confirm == "":
        showMessage("Please answer with y or n.", "warning")
        return getYesNoAnswer(question)

    # true if user replied with yes or y
    return (toLowerAscii(confirm) == "y") or (toLowerAscii(confirm) == "yes")

proc displayHelp*() =
    # https://git.sr.ht/~reesmichael1/nim-pager
    echo """
    This CLI application is intended to help you play music in the background from YouTube.

    Now, let's look at the menu and talk about it:
    
    Play Playlist: This option will read from the playlists saved inside
    'custom-playlists.txt' and display the options to you. You can play one or more
    playlists (separated by spaces) from here.
    
    Add Playlist (online): If you want to add a new playlist to your list You can
    insert a playlist name which should NOT contain any '~' or '`'. Then you can
    insert a playlist link copied from YouTube. The playlist should be added to the
    file with no problem.
    
    Remove Playlist: If there is a playlist you don't like using, just remove it
    from the list. If you want to listen to it after deleting it, you'll have to add
    it again.
    
    Edit Playlist: Did you perhaps insert the wrong name or link for your playlist?
    Or did the link change? This option will allow you to edit the playlist name and
    link.
    
    Flags: There are 3 types of flags you can add when running bgmplayer,
    single-answer flags (--), multi-answer flags (-) and the playlist flag (playlist
    number). These flags can be used to play playlists without loading the rest of
    the interface. You can add the playlist number to the end of the command
    (eg. bgmplayer 1) and it will instantly play and shuffle the playlist. You can
    use -no-shuffle to not shuffle the playlist (so: bgmplayer 1 -no-shuffle). So
    far we've talked abut multi-answer flags, flags where you can add multiple
    flags. Single-answer flags are different in the sense that they use 2 '--'
    instead of 1 '-' and that there can only be 1 flag if it's a single-answer flag
    (eg. bgmplayer --help). You can find out more about flags in the Github page.
    """

    showMessage("\nSome problems may still occur and if it does, edit the code yourself if you have\nthe source code or remove the problem from the 'custom-playlists.txt' file (if\nthe problem is from the playlists file), or create an issue on Github:\n\thttps://github.com/WeebNetsu/yt-playlist-bgm-player", "notice")
