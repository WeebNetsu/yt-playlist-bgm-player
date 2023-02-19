import std/strformat
from std/os import getConfigDir, joinPath, existsOrCreateDir, fileExists, normalizedPath
from std/strutils import parseInt, toLowerAscii, strip, replace
from std/terminal import setForegroundColor, resetAttributes, ForegroundColor
import pkg/ncurses

type MessageType* = enum SUCCESS, WARN, NOTICE

const
    # the folder at which any files related to the program gets saved
    saveFolderName*: string = "ytbgmpcli"
    # name of the file that stores playlist information
    saveFileName*: string = "playlists.json"

# path to the save file
let appSaveFile*: string = joinPath(getConfigDir(), saveFolderName, saveFileName)

proc displayMenu*(menuOptions: openArray[string]) =
    ## Display a menu with numbers at the front and ending with "0. Exit"
    echo "What would you like to do?"

    for index, option in menuOptions:
        echo &"{index + 1}. {option}"

    echo "0. Exit"

proc cleanFilePath*(path: string): string =
    # if a local playlist
    if os.isAbsolute(path):
        return normalizedPath(path).replace(" ", "\\ ").joinPath()

    return path

proc criticalError*(errorMessage: string) =
    # todo display message in red
    setForegroundColor(ForegroundColor.fgRed)
    echo &"Critical Error: {errorMessage}"
    stdout.resetAttributes() # reset terminal colors & stuff
    system.quit()

proc showMessage*(msg: string, msgType: MessageType = MessageType.SUCCESS) =
    case msgType:
        of MessageType.NOTICE:
            setForegroundColor(ForegroundColor.fgBlue)
            echo msg
        of MessageType.WARN:
            setForegroundColor(ForegroundColor.fgYellow)
            # only warning and errors will prepended to the message
            echo "Warning: ", msg
        of MessageType.SUCCESS:
            setForegroundColor(ForegroundColor.fgGreen)
            echo msg

    stdout.resetAttributes() # reset terminal colors & stuff

proc setup*(): bool =
    let configDir = joinPath(getConfigDir(), saveFolderName)

    try:
        # check if dir exists, if not, create it
        if not existsOrCreateDir(configDir):
            showMessage("Config folder not found, creating it...", MessageType.NOTICE)
    except OSError:
        showMessage(&"Failed to create config directory {configDir}", MessageType.WARN)
        return false

    try:
        if not fileExists(appSaveFile):
            showMessage("Save file not found, creating it...", MessageType.NOTICE)
            writeFile(appSaveFile, "[]")
    except OSError:
        showMessage(&"Failed to create config file: {appSaveFile}", MessageType.WARN)
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
            showMessage("Invalid option... Does it exist?", MessageType.WARN)
            return getSelectableOption(question, options, inputStr)

        return chosenOption
    except ValueError:
        echo "Invalid choice"
        return getSelectableOption(question, options, inputStr)
    except:
        utils.criticalError(&"Unknown error while getting user input.")

proc getYesNoAnswer*(question: string): bool =
    # we will always default to no, so if the user inputs
    # 's' or 'a' or ' ' or 'n', they will all be seen as
    # false
    stdout.write(question & " [y/N]: ")

    let confirm: string = readLine(stdin).strip()

    # if confirm == "":
    #     showMessage("Please answer with y or n.", MessageType.WARN)
    #     return getYesNoAnswer(question)

    # true if user replied with yes or y
    return (toLowerAscii(confirm) == "y") or (toLowerAscii(confirm) == "yes")

proc getKeyPress*(): int =
    # this function just returns the key that was pressed
    var pwin = initscr()
    raw()
    keypad(pwin, true)
    noecho()

    let ch: cint = getch()

    endwin()
    return int(ch)

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
    
    Flags: You can find out more about available flags in the Github page or by running ytbgmplayer -h.
    """

    showMessage(
            &"\nSome problems may still occur and if it does, edit the code yourself if you have\nthe source code or remove the problem from the '{saveFileName}' file (if\nthe problem is from the playlists file), or create an issue on Github:\n\thttps://github.com/WeebNetsu/yt-playlist-bgm-player",
            MessageType.NOTICE
        )
