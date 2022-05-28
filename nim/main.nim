#[ import cligen
from os import paramCount

proc main(foo=1,bar=2.0,baz="hi",verb=false,paths: seq[string]):int=
    var running: bool = false
    let menuOptions: array[5, string] = ["Play Playlists", "Add Playlist", "Edit Playlist", "Remove Playlist", "Help"]

    echo paramCount()

    echo paths

dispatch(main) ]#

from os import paramCount
from strutils import parseInt
import utils, player

proc main() =
    const menuOptions: array[5, string] = ["Play Playlists", "Add Playlist", "Edit Playlist", "Remove Playlist", "Help"]

    echo "Welcome!"

    # if setup fails, we will not run the program
    var running: bool = utils.setup()

    while running:
        if paramCount() == 0:
            utils.displayMenu(menuOptions)

        try:
            stdout.write("> ")
            let choice: int = readLine(stdin).parseInt()

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
                of 4:
                    utils.showMessage("Remove Playlist", "notice")
                of 5:
                    utils.showMessage("Help", "notice")
                else:
                    echo "Invalid Choice"
        except ValueError:
            echo "Invalid choice"
            continue
        except:
            echo "Unknown error"
            system.quit()

    echo "Goodbye!"

main()
