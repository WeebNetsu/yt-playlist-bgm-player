from std/os import paramCount
import pkg/[cligen, mpv, ncurses], std/strformat
from std/sequtils import map
from std/random import randomize

import utils, player

# if the user ctrl+c out of the application
system.setControlCHook(proc() {.noconv.} = criticalError("Unexpected exit! (Please use 'q' instead)"))

randomize()

proc keyPress(): int =
    # this function just returns the key that was pressed
    var pwin = initscr()
    raw()
    keypad(pwin, true)
    noecho()

    let ch: cint = getch()

    endwin()
    return int(ch)


proc checkMpvError(errno: cint) =
    if errno < 0:
        echo("Happy Error")

# var bool -> reference (would be `int &paused` in C++)
proc manageMPVKeyPress(mpvCtx: auto, paused: var bool, muted: var bool): bool =
    # returning false means quite the application
    let kp = keyPress()

    # we can put all our key press events here!
    case kp
    of ord(' '):
        if paused:
            checkMpvError(mpvCtx.set_option_string("pause", "no"))
        else:
            checkMpvError(mpvCtx.set_option_string("pause", "yes"))

        paused = not paused
    of ord('q'):
        return false
    of ord('>'):
        mpvCtx.command("playlist-next", "force")
    of ord('<'):
        mpvCtx.command("playlist-prev", "force")
    of ord('m'):
        mpvCtx.set_option("mute", if muted: "no" else: "yes")
        muted = not muted
    of 258: # down
        mpvCtx.command("seek", "-60")
    of 259: # up
        mpvCtx.command("seek", "60")
    of 260: # left
        mpvCtx.command("seek", "-10")
    of 261: # right
        mpvCtx.command("seek", "10")
    else:
        return true
    return true

proc main(playlists: seq[int], shuffle = false, loop = false, list = false) =
    const menuOptions: array[5, string] = ["Play Playlists", "Add Playlist",
            "Edit Playlist", "Remove Playlist", "Help"]

    utils.showMessage("Welcome!", utils.MessageType.SUCCESS)

    # Create the MPV context to use MPV
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
    mpvCtx.set_option("script-opts", "ytdl_hook-ytdl_path=/usr/bin/yt-dlp")
    # mpvCtx.set_option("loop-playlist", "yes")
    mpvCtx.set_option("vid", "no") # disable video playback

    if shuffle:
        mpvCtx.set_option("shuffle", "yes")
        utils.showMessage("Shuffling playlists", utils.MessageType.NOTICE)

    if loop:
        mpvCtx.set_option("loop-playlist", "yes")
        utils.showMessage("Looping playlists", utils.MessageType.NOTICE)

    # check for any errors before continuing
    check_error(mpvCtx.initialize())

    var
        # if setup fails, we will not run the program
        running: bool = utils.setup()
        playing: bool = false
        # MPV playing options
        paused: bool = false
        muted: bool = false

    while running:
        if playing:
            let event = mpvCtx.wait_event(10000)

            if not manageMPVKeyPress(mpvCtx, paused, muted):
                break

            if event.event_id == mpv.EVENT_SHUTDOWN:
                break

            continue

        if paramCount() == 0:
            let choice = utils.getSelectableOption("", menuOptions)

            case choice:
                of 0:
                    running = false
                of 1:
                    utils.showMessage("Play Playlist", utils.MessageType.NOTICE)
                    player.playPlaylists(mpvCtx)
                    playing = true
                of 2:
                    utils.showMessage("Add Playlist", utils.MessageType.NOTICE)
                    player.addPlaylist()
                of 3:
                    utils.showMessage("Edit Playlist", utils.MessageType.NOTICE)
                    player.updatePlaylist()
                of 4:
                    utils.showMessage("Remove Playlist", utils.MessageType.NOTICE)
                    player.removePlaylist()
                of 5:
                    utils.displayHelp()
                else:
                    # user should never get to this point
                    echo "Invalid Choice"
        else:
            if list:
                echo "Available Playlists:"
                for index, item in player.getPlaylists():
                    echo &"{index+1}. {item.name}"

                # list cannot be used with other flags
                break

            # when a user decides to use the random flag, they cannot specify what playlists to play
            # randomly. To play specific playlists randomly, they can use the shuffle flag
            # if random:
            #     playing = true
            #     player.instantPlayPlaylists(mpvCtx, [])

            if len(playlists) > 0:
                if playlists.find(0) >= 0:
                    utils.showMessage("0 is not a valid playlist number", utils.MessageType.WARN)
                    break

                playing = true
                player.instantPlayPlaylists(mpvCtx, map(playlists, proc(val: int): int = val - 1))


            if not playing:
                running = false

    mpv.terminate_destroy(mpvCtx)

    utils.showMessage("\nGoodbye!", utils.MessageType.NOTICE)

dispatch(main, help = {
    "playlists": "Selected playlist(s) to play",
    # "random": "Play all your playlists in random order",
    "shuffle": "Enable playlist shuffling",
    "loop": "Enable playlist looping",
    "list": "Return list of available playlists",
})
