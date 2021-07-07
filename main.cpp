#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "include/FileManager.hpp"
#include "np/Color.hpp"

void displayMenu(std::vector<std::string> menuOptions);
void processAnswer(int option, std::vector<std::string> menuOptions, FileManager *fm);
void manageFlags(std::vector<std::string> &arguments, FileManager &fm);
void displayHelp(FileManager *fm);

void showMessage(std::string message, std::string type = "normal", bool breakLine = true)
{
    Color::Modifier defColor = Color::FG_DEFAULT; // normal

    if (type == "error")
    {
        defColor = Color::FG_RED;
    }
    else if (type == "success")
    {
        defColor = Color::FG_GREEN;
    }
    else if (type == "warning")
    {
        defColor = Color::FG_YELLOW;
    }
    else if (type == "notice")
    {
        defColor = Color::FG_BLUE;
    }

    if (breakLine)
    {
        std::cout << defColor << message << std::endl;
    }
    else
    {
        std::cout << defColor << message;
    }

    //reset the console color
    defColor = Color::FG_DEFAULT;
    std::cout << defColor;
}

bool running = true;

int main(int argc, char const *argv[])
{
    std::vector<std::string> menuOptions = {"Play Playlists", "Add Playlist", "Edit Playlist", "Remove Playlist", "Help"};

    showMessage("Welcome!");

    FileManager fm("custom-playlists.txt");

    // single flag options will not execute any other commands other than itself.
    // we use vector since we can for through the array and check if anything is found
    std::vector<std::string> singleFlagOptions = {"--help", "--list"};
    std::vector<std::string> arguments;

    for (int i = 1; i < argc; i++) // so we don't receive the file name
    {
        arguments.push_back(argv[i]);
    }

    if (arguments.size() > 0) // if arguments were passed in
    {
        manageFlags(arguments, fm);

        return 0;
    }

    while (running)
    {
        if (fm.checkFileEmpty())
        {
            showMessage("Note: Custom playlists file is empty", "warning");
        }

        std::cout << std::endl;

        displayMenu(menuOptions);
        int option;

        std::cout << "> ";
        std::cin >> option;

        processAnswer(option, menuOptions, &fm);
    }

    return 0;
}

// get the choice the user made in the menu
void processAnswer(int option, std::vector<std::string> menuOptions, FileManager *fm)
{
    switch (option)
    {
    case 1:
        showMessage("Play Playlists");
        fm->playPlaylists();
        break;

    case 2:
        showMessage("Add Playlist");
        fm->addPlaylist();
        break;

    case 3:
        showMessage("Update Playlist");
        fm->updatePlaylist();
        break;

    case 4:
        showMessage("Remove Playlist");
        fm->removePlaylist();
        break;

    case 5:
        std::cout << std::endl;
        displayHelp(fm);
        std::cout << std::endl;
        break;

    case 0:
        showMessage("Exit");
        running = false;
        break;

    default:
        showMessage("Invalid input... Please try again: ", "warning");
        std::cin.clear();
        std::cin.ignore(100, '\n');
        break;
    }
}

void manageFlags(std::vector<std::string> &arguments, FileManager &fm)
{
    std::vector<std::string> singleFlagOptions = {"--help", "--list"};

    std::string args_1 = arguments[0];
    int playlistToPlay = -1;
    std::vector<int> playlists;

    std::map<std::string, bool> playerFlagput = {
        {"shuffle", true},
        {"loop", true},
        {"singleFlag", false}};

    for (std::string &i : singleFlagOptions)
    {
        for (std::string &j : arguments)
        {
            if (i == j)
            {
                playerFlagput["singleFlag"] = true;
            }
        }
    }

    if (playerFlagput["singleFlag"])
    {
        if (arguments.size() > 1)
        {
            showMessage("Single answer flag detected (any flag starting with '--'). No other flags or arguments may be added when adding a single answer flag", "error");
        }
        else
        {
            for (std::string &i : arguments)
            {
                if (i == singleFlagOptions.at(0)) // --help
                {
                    displayHelp(&fm);
                }
                else if (i == singleFlagOptions.at(1)) // --list
                {
                    showMessage("Your Playlists:");

                    fm.showPlaylists();
                }
            }
        }
    }
    else
    {
        for (std::string &i : arguments)
        {
            if (i == "-no-shuffle")
            {
                playerFlagput["shuffle"] = false;
                showMessage("Not shuffling playlist.", "success");
            }
            else if (i == "-no-loop")
            {
                showMessage("Not looping playlist.", "success");
                playerFlagput["loop"] = false;
            }
            else
            {
                try
                {
                    playlistToPlay = std::stoi(i);

                    if (playlistToPlay < 1) // playlists starts at 1
                    {
                        throw 1;
                    }

                    playlists.push_back(playlistToPlay);
                }
                catch (...)
                {
                    showMessage("Error with argument input. \"" + i + "\" is either an invalid argument, or a number lower than 1.", "error");

                    return;
                }
            }
        }

        fm.instantPlayPlaylists(playlists, playerFlagput);
    }
}

void displayHelp(FileManager *fm)
{
    std::string helpText = "";
    helpText += "This CLI application is intended to help you play music in the background from YouTube.\n";

    helpText += "Now, let's look at the menu and talk about it:\n";

    helpText += "\n\tPlay Playlist: This option will read from the playlists saved inside\n\t'custom-playlists.txt' and display the options to you. You can play one or more\n\tplaylists (separated by spaces) from here.";

    helpText += "\n\tAdd Playlist (online): If you want to add a new playlist to your list You can\n\tinsert a playlist name which should NOT contain any '~' or '`'. Then you can\n\tinsert a playlist link copied from YouTube. The playlist should be added to the\n\tfile with no problem.\n";

    helpText += "\n\tRemove Playlist: If there is a playlist you don't like using, just remove it\n\tfrom the list. If you want to listen to it after deleting it, you'll have to add\n\tit again.\n";

    helpText += "\n\tEdit Playlist: Did you perhaps insert the wrong name or link for your playlist?\n\tOr did the link change? This option will allow you to edit the playlist name and\n\tlink.\n";

    helpText += "\n\tFlags: There are 3 types of flags you can add when running bgmplayer,\n\tsingle-answer flags (--), multi-answer flags (-) and the playlist flag (playlist\n\tnumber). These flags can be used to play playlists without loading the rest of\n\tthe interface. You can add the playlist number to the end of the command\n\t(eg. bgmplayer 1) and it will instantly play and shuffle the playlist. You can\n\tuse -no-shuffle to not shuffle the playlist (so: bgmplayer 1 -no-shuffle). So\n\tfar we've talked abut multi-answer flags, flags where you can add multiple\n\tflags. Single-answer flags are different in the sense that they use 2 '--'\n\tinstead of 1 '-' and that there can only be 1 flag if it's a single-answer flag\n\t(eg. bgmplayer --help). You can find out more about flags in the Github page.\n";

    helpText += "\nPress 'q' to exit help.";

    std::ofstream fHelpFile;
    fHelpFile.open(fm->getFileName("help.txt"));

    if (!fHelpFile.is_open())
    {
        showMessage("Couldn't open help file for short reading. Printing default.", "warning");
        showMessage(helpText);

        return;
    }

    fHelpFile << helpText;
    fHelpFile.close();

    std::string command = "less ";
    command += fm->getFileName("help.txt");

    const char *execute = command.c_str();

    system(execute);

    fm->displayPlayerControls();

    showMessage("\nSome problems may still occur and if it does, edit the code yourself if you have\nthe source code or remove the problem from the 'custom-playlists.txt' file (if\nthe problem is from the playlists file), or create an issue on Github:\n\thttps://github.com/WeebNetsu/yt-playlist-bgm-player", "notice");
}

// display the menu to the user
void displayMenu(std::vector<std::string> menuOptions)
{
    std::cout << "What would you like to do?" << std::endl;

    for (int i = 0; i < menuOptions.size(); i++)
    {
        std::cout << i + 1 << ". " << menuOptions.at(i) << std::endl;
    }

    std::cout << "0. Exit" << std::endl;
}