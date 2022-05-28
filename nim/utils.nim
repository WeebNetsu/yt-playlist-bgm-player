import strformat
from os import getConfigDir, joinPath, existsOrCreateDir, fileExists

const 
    scriptOpts*: string = "--script-opts=ytdl_hook-ytdl_path=/usr/local/bin/yt-dlp"
    saveFolderName*: string = "ytbgmpcli"
    saveFileName*: string = "playlists.txt"

let appSaveFile*: string = joinPath(getConfigDir(), saveFolderName, saveFileName)

from terminal import setForegroundColor, resetAttributes, ForegroundColor

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
