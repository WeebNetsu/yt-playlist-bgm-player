from std/os import paramCount
import pkg/[cligen, mpv], std/strformat
from std/sequtils import map
from std/random import randomize

import utils, player, mpvconf

# if the user ctrl+c out of the application
# todo make it quit safely
system.setControlCHook(proc() {.noconv.} = utils.criticalError("Unexpected exit! (Please use 'q' instead)"))

randomize()

proc main(playlists: seq[int], shuffle = false, loop = false, list = false) =
    detectDependencies()

    const menuOptions: array[5, string] = ["Play Playlists", "Add Playlist",
            "Edit Playlist", "Remove Playlist", "Help"]

    utils.showMessage("Welcome!", utils.MessageType.SUCCESS)

    # Create the MPV context to use MPV
    let mpvCtx = mpvconf.initMpvCtx(shuffle, loop)

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

            if not mpvconf.manageMPVKeyPress(mpvCtx, paused, muted):
                break

            if event.event_id == mpv.EVENT_SHUTDOWN:
                break

            continue

        if os.paramCount() == 0:
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

cligen.dispatch(main, help = {
    "playlists": "Selected playlist(s) to play",
    # "random": "Play all your playlists in random order",
    "shuffle": "Enable playlist shuffling",
    "loop": "Enable playlist looping",
    "list": "Return list of available playlists",
})
