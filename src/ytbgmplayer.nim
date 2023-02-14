from std/os import paramCount
import pkg/[cligen, mpv], std/strformat
from std/sequtils import map
from std/random import randomize

import utils, player

# if the user ctrl+c out of the application
system.setControlCHook(proc() {.noconv.} = criticalError("Unexpected exit! (Please use 'q' instead)"))

randomize()

proc main(playlists: seq[int], random = false, shuffle = false,
        loop = false, list = false) =
    const menuOptions: array[5, string] = ["Play Playlists", "Add Playlist",
            "Edit Playlist", "Remove Playlist", "Help"]

    utils.showMessage("Welcome!", utils.MessageType.SUCCESS)

    # Create the MPV context to use MPV
    let ctx = mpv.create()
    if ctx.isNil:
        echo "failed creating mpv context"
        return

    # set the nim context options
    # Enable default key bindings, so the user can actually interact with
    # the player (and e.g. close the window).
    ctx.set_option("terminal", "yes")
    ctx.set_option("input-default-bindings", "yes")
    ctx.set_option("input-vo-keyboard", "yes")
    ctx.set_option("osc", true)
    # ctx.set_option("loop-playlist", "yes")
    ctx.set_option("vid", "no") # disable video playback

    # check for any errors before continuing
    check_error(ctx.initialize())

    # if setup fails, we will not run the program
    var running: bool = utils.setup()

    while running:
        if paramCount() == 0:
            let choice = utils.getSelectableOption("", menuOptions)

            case choice:
                of 0:
                    running = false
                of 1:
                    utils.showMessage("Play Playlist", utils.MessageType.NOTICE)
                    player.playPlaylists()
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
            if random:
                # if len(playlists) > 0:
                #     if playlists.find(0) >= 0:
                #         utils.showMessage("0 is not a valid playlist number", utils.MessageType.WARN)
                #         break

                player.instantPlayPlaylists([], true, true, true)

            if len(playlists) > 0:
                if playlists.find(0) >= 0:
                    utils.showMessage("0 is not a valid playlist number", utils.MessageType.WARN)
                    break

                player.instantPlayPlaylists(map(playlists, proc(
                        val: int): int = val - 1), shuffle, loop)

            running = false

    utils.showMessage("\nGoodbye!", utils.MessageType.NOTICE)

dispatch(main, help = {
    "playlists": "Selected playlist(s) to play",
    "random": "Play all your playlists in random order",
    "shuffle": "Enable playlist shuffling",
    "loop": "Enable playlist looping",
    "list": "Return list of available playlists",
})
