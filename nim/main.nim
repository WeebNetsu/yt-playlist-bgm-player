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
    var running: bool = player.setup()

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
                    echo "[TODO] Play Playlists"
                of 2:
                    player.addPlaylist()
                of 3:
                    echo "[TODO] Edit Playlist"
                of 4:
                    echo "[TODO] Remove Playlist"
                of 5:
                    echo "[TODO] Help"
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
