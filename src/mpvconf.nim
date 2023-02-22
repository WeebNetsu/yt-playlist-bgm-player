from std/os import findExe
import std/strformat
import pkg/mpv

import utils

# returns auto, since idk what type mpv.create() actually returns
proc initMpvCtx*(shuffle: bool, loop: bool): auto =
    let mpvCtx = mpv.create()

    if mpvCtx.isNil:
        echo "failed creating mpv context"
        return

    # set the nim context options
    # Enable default key bindings, so the user can actually interact with
    # the player (and e.g. close the window).
    mpvCtx.set_option("terminal", "yes")
    mpvCtx.set_option("input-default-bindings", "yes")
    mpvCtx.set_option("input-vo-keyboard", "yes")
    mpvCtx.set_option("osc", true)
    mpvCtx.set_option("ytdl", "yes")
    mpvCtx.set_option("script-opts", &"""ytdl_hook-ytdl_path={os.findExe("yt-dlp")}""")
    # mpvCtx.set_option("loop-playlist", "yes")
    mpvCtx.set_option("vid", "no") # disable video playback

    if shuffle:
        mpvCtx.set_option("shuffle", "yes")
        utils.showMessage("Shuffling playlists", utils.MessageType.NOTICE)

    if loop:
        mpvCtx.set_option("loop-playlist", "yes")
        utils.showMessage("Looping playlists", utils.MessageType.NOTICE)

    # check for any errors before continuing
    mpv.check_error(mpvCtx.initialize())

    return mpvCtx

proc checkMpvError*(errno: cint) =
    if errno < 0:
        # todo user will not be as happy as this error lol
        echo("Happy Error")

# var bool -> reference (would be `int &paused` in C++)
proc manageMPVKeyPress*(mpvCtx: auto, paused: var bool, muted: var bool): bool =
    # returning false means quite the application
    let keyPress = utils.getKeyPress()

    # we can put all our key press events here!
    case keyPress
    of ord(' '):
        if paused:
            checkMpvError(mpvCtx.set_option_string("pause", "no"))
        else:
            checkMpvError(mpvCtx.set_option_string("pause", "yes"))

        paused = not paused
    of ord('q'):
        # quit the program
        return false
    of ord('>'):
        # next song
        mpvCtx.command("playlist-next", "force")
    of ord('<'):
        # previous song
        mpvCtx.command("playlist-prev", "force")
    of ord('m'):
        # mute
        mpvCtx.set_option("mute", if muted: "no" else: "yes")
        muted = not muted
    of 258: # down
        # rewind 1 min
        mpvCtx.command("seek", "-60")
    of 259: # up
        # forward 1 min
        mpvCtx.command("seek", "60")
    of 260: # left
        # rewind 10 seconds
        mpvCtx.command("seek", "-10")
    of 261: # right
        # forward 10 seconds
        mpvCtx.command("seek", "10")
    else:
        return true
    return true
