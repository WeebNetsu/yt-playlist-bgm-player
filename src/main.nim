from os import paramCount
import cligen, strformat
from sequtils import map
from random import randomize

import utils, player

# if the user ctrl+c out of the application
system.setControlCHook(proc() {.noconv.} = criticalError("Unexpected exit!"))

randomize()

proc main(playlists: seq[int], list=false, random=false, noShuffle=false, noLoop=false) =
    const menuOptions: array[5, string] = ["Play Playlists", "Add Playlist", "Edit Playlist", "Remove Playlist", "Help"]

    utils.showMessage("Welcome!\n", "success")

    # if setup fails, we will not run the program
    var running: bool = utils.setup()

    while running:
        if paramCount() == 0:
            let choice = utils.getSelectableOption("", menuOptions)

            case choice:
                of 0:
                    running = false
                of 1:
                    utils.showMessage("Play Playlist", "notice")
                    player.playPlaylists()
                of 2:
                    utils.showMessage("Add Playlist", "notice")
                    player.addPlaylist()
                of 3:
                    utils.showMessage("Edit Playlist", "notice")
                    player.updatePlaylist()
                of 4:
                    utils.showMessage("Remove Playlist", "notice")
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

            if random:
                if len(playlists) > 0:
                    if playlists.find(0) >= 0:
                        utils.showMessage("0 is not a valid playlist number", "warning")
                        break

                player.instantPlayPlaylists([], true, true, true)

            if len(playlists) > 0:
                if playlists.find(0) >= 0:
                    utils.showMessage("0 is not a valid playlist number", "warning")
                    break

                player.instantPlayPlaylists(map(playlists, proc(val: int): int = val - 1), not noShuffle, not noLoop)

            running = false

    utils.showMessage("\nGoodbye!", "notice")

dispatch(main, help={
    "playlists": "Selected playlist(s) to play",
    "list": "Return list of available playlists",
    "random": "Play all your playlists in random order",
    "no-shuffle": "Disables playlist shuffling",
    "no-loop": "Disables playlist looping",
})
