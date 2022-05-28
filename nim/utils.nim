import strformat

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