#[ import cligen
from os import paramCount

proc main(foo=1,bar=2.0,baz="hi",verb=false,paths: seq[string]):int=
    var running: bool = false
    let menuOptions: array[5, string] = ["Play Playlists", "Add Playlist", "Edit Playlist", "Remove Playlist", "Help"]

    echo paramCount()

    echo paths

dispatch(main) ]#

from os import paramCount

import utils, player

proc main() =
    const menuOptions: array[5, string] = ["Play Playlists", "Add Playlist", "Edit Playlist", "Remove Playlist", "Help"]

    utils.showMessage("Welcome!", "success")

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
                of 2:
                    utils.showMessage("Add Playlist", "notice")
                    player.addPlaylist()
                of 3:
                    utils.showMessage("Edit Playlist", "notice")
                    player.updatePlaylist()
                of 4:
                    utils.showMessage("Remove Playlist", "notice")
                of 5:
                    utils.displayHelp()
                else:
                    # user should never get to this point
                    echo "Invalid Choice"
        else:
            running = false

    utils.showMessage("Goodbye!", "notice")

main()
