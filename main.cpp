#include <iostream>
#include <fstream>
#include <vector>

#include "FileManager.hpp"

void displayMenu(std::vector<std::string> menuOptions);
void processAnswer(int option, std::vector<std::string> menuOptions, FileManager *fm);
void displayHelp(FileManager *fm);
bool running = true;

int main(int argc, char const *argv[])
{
    std::vector<std::string> menuOptions = {"Play Playlist", "Add Playlist", "Remove Playlist", "Edit Playlist", "Help"};

    std::cout << "Welcome!" << std::endl;

    FileManager fm("custom-playlists.txt");

    std::vector<std::string> flagOptions = {"-no-shuffle"};
    // single flag options will not execute any other commands other than itself.
    std::vector<std::string> singleFlagOptions = {"--help"};
    std::vector<std::string> arguments;

    for (int i = 1; i < argc; i++) // so we don't receive the file name
    {
        arguments.push_back(argv[i]);
    }

    if ((arguments.size() > 0) && (arguments.size() <= flagOptions.size() + 1))
    { // if arguments were passed in
        std::string args_1 = arguments[0];
        int playlistToPlay;

        bool shufflePlaylist = true;
        bool singleFlag = false;
        for (std::string &i : singleFlagOptions)
        {
            for (std::string &j : arguments)
            {
                if (i == j)
                {
                    singleFlag = true;
                }
            }
        }

        if (singleFlag)
        {
            if (arguments.size() > 1)
            {
                std::cout << "Single answer flag detected (any flag starting with '--'). No other flags or arguments may be added when adding a single answer flag" << std::endl;
            }
            else
            {
                for (std::string &i : arguments)
                {
                    if (i == singleFlagOptions.at(0)) // --help
                    {
                        displayHelp(&fm);
                    }
                }
            }

            return 0;
        }
        else
        {
            for (std::string &i : arguments)
            {
                if (i == flagOptions.at(0)) // -no-shuffle
                {
                    shufflePlaylist = false;
                    std::cout << "Not shuffling playlist." << std::endl;
                }
                else
                {                           // they have most likely passed in a number
                    if (playlistToPlay > 0) // if a number was already passed in
                    {
                        std::cout << "Found multiple playlist numbers to play. Please only insert 1 number." << std::endl;
                        return 0;
                    }

                    try
                    {
                        playlistToPlay = std::stoi(i);

                        if (playlistToPlay < 1) // playlists starts at 1
                        {
                            throw 1;
                        }
                    }
                    catch (...)
                    {
                        std::cout << "Error with argument input. " << i << " is either an invalid argument, or a number lower than 1." << std::endl;

                        return 0;
                    }
                }
            }

            fm.instantPlayPlaylist(playlistToPlay, shufflePlaylist); // play and shuffle playlist
        }

        return 0;
    }
    else if (argc > flagOptions.size() + 2)
    {
        std::cout << "Too many arguments passed in, not parsing." << std::endl;
        return 0;
    }

    while (running)
    {
        if (fm.checkFileEmpty())
        {
            std::cout << "Note: Custom playlists file is empty" << std::endl;
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

void processAnswer(int option, std::vector<std::string> menuOptions, FileManager *fm)
{
    switch (option)
    {
    case 1:
        std::cout << "Play Playlist" << std::endl;
        fm->playPlaylist();
        break;

    case 2:
        std::cout << "Add Playlist" << std::endl;
        fm->addPlaylist();
        break;

    case 3:
        std::cout << "Remove Playlist" << std::endl;
        fm->removePlaylist();
        break;

    case 4:
        std::cout << "Update Playlist" << std::endl;
        fm->updatePlaylist();
        break;

    case 5:
        std::cout << std::endl;
        displayHelp(fm);
        std::cout << std::endl;
        break;

    case 0:
        std::cout << "Exit" << std::endl;
        running = false;
        break;

    default:
        std::cout << "Invalid input... Please try again: ";
        std::cin.clear();
        std::cin.ignore(100, '\n');
        break;
    }
}

void displayHelp(FileManager *fm)
{
    // std::cout << "Welcome!" << std::endl;
    // std::cout << "This CLI application is intended to help you play music in the background from YouTube." << std::endl;
    // std::cout << "Now, let's look at the menu and talk about it:\n"
    //           << std::endl;
    // std::cout << "\tPlay Playlist: This option will read from the playlists saved inside\n\t'custom-playlists.txt' and display the options to you. You can choose a\n\tplaylist, and if everyting has been inserted correctly and the playlist still\n\texists, it will play the playlist (looping the playlist)\n"
    //           << std::endl;
    // std::cout << "\tAdd Playlist (online): If you want to add a new playlist to your list You can\n\tinsert a playlist name which should NOT contain any '~' or '`'. Then you can\n\tinsert a playlist link copied from YouTube. The playlist should be added to the\n\tfile with no problem.\n"
    //           << std::endl;
    // std::cout << "\tAdd Playlist (local): If you want to add a new playlist to your list You can\n\tinsert a playlist name which should NOT contain any '`' or '~'. Then you can\n\tinsert a playlist location, copied from a pwd result you typed in the terminal\n\t(go to folder with music in it. Type pwd. Copy the result and paste it into the\n\tlocal playlist location option). The playlist should be added to the file with no\n\tproblem.\n"
    //           << std::endl;
    // std::cout << "\tRemove Playlist: If there is a playlist you don't like using, just remove it\n\tfrom the list. If you want to listen to it after deleting it, you'll have to add\n\tit again.\n"
    //           << std::endl;
    // std::cout << "\tEdit Playlist: Did you perhaps insert the wrong name or link for your playlist?\n\tOr did the link change? This option will allow you to edit the playlist name and\n\tlink.\n"
    //           << std::endl;
    // std::cout << "\tHelp: Display some help you may need.\n"
    //           << std::endl;
    // std::cout << "\tFlags: There are 3 types of flags you can add when running bgmplayer,\n\tsingle-answer flags (--), multi-answer flags (-) and the playlist flag (playlist\n\tnumber). These flags can be used to play playlists without loading the rest of\n\tthe interface. You can add the playlist number to the end of the command\n\t(eg. bgmplayer 1) and it will instantly play and shuffle the playlist. You can\n\tuse -no-shuffle to not shuffle the playlist (so: bgmplayer 1 -no-shuffle). So\n\tfar we've talked abut multi-answer flags, flags where you can add multiple\n\tflags. Single-answer flags are different in the sense that they use 2 '--'\n\tinstead of 1 '-' and that there can only be 1 flag if it's a single-answer flag\n\t(eg. bgmplayer --help). You can find out more about flags in the Github page." << std::endl;
    std::string helpText = "";
    helpText += "This CLI application is intended to help you play music in the background from YouTube.\n";
    helpText += "Now, let's look at the menu and talk about it:\n";
    helpText += "\n\tPlay Playlist: This option will read from the playlists saved inside\n\t'custom-playlists.txt' and display the options to you. You can choose a\n\tplaylist, and if everyting has been inserted correctly and the playlist still\n\texists, it will play the playlist (looping the playlist)\n";
    helpText += "\n\tAdd Playlist (online): If you want to add a new playlist to your list You can\n\tinsert a playlist name which should NOT contain any '~' or '`'. Then you can\n\tinsert a playlist link copied from YouTube. The playlist should be added to the\n\tfile with no problem.\n";
    helpText += "\n\tRemove Playlist: If there is a playlist you don't like using, just remove it\n\tfrom the list. If you want to listen to it after deleting it, you'll have to add\n\tit again.\n";
    helpText += "\n\tEdit Playlist: Did you perhaps insert the wrong name or link for your playlist?\n\tOr did the link change? This option will allow you to edit the playlist name and\n\tlink.\n";
    helpText += "\n\tFlags: There are 3 types of flags you can add when running bgmplayer,\n\tsingle-answer flags (--), multi-answer flags (-) and the playlist flag (playlist\n\tnumber). These flags can be used to play playlists without loading the rest of\n\tthe interface. You can add the playlist number to the end of the command\n\t(eg. bgmplayer 1) and it will instantly play and shuffle the playlist. You can\n\tuse -no-shuffle to not shuffle the playlist (so: bgmplayer 1 -no-shuffle). So\n\tfar we've talked abut multi-answer flags, flags where you can add multiple\n\tflags. Single-answer flags are different in the sense that they use 2 '--'\n\tinstead of 1 '-' and that there can only be 1 flag if it's a single-answer flag\n\t(eg. bgmplayer --help). You can find out more about flags in the Github page.\n";
    helpText += "\nPress 'q' to exit help.";

    std::ofstream fHelpFile;
    fHelpFile.open(fm->getTempFileName("help.txt"));

    if (!fHelpFile.is_open())
    {
        std::cout << "Couldn't open help file for short reading. Printing default." << std::endl;
        std::cout << helpText << std::endl;

        return;
    }

    fHelpFile << helpText;
    fHelpFile.close();

    // std::string line;
    // std::ifstream fPlaylistFile(fm->getTempFileName("help.txt")); //gets input from the file

    std::string command = "less ";
    command += fm->getTempFileName("help.txt");

    // fPlaylistFile.close();

    const char *execute = command.c_str();

    system(execute);

    fm->displayPlayerControls();

    std::cout << "\nSome problems may still happen, so if it does, edit the code yourself if you have the\nsource code or remove the problem from the 'custom-playlists.txt' file (if the problem\nis from the playlists file), or create an issue on Github: https://github.com/WeebNetsu/yt-playlist-bgm-player" << std::endl;
}

void displayMenu(std::vector<std::string> menuOptions)
{
    std::cout << "What would you like to do?" << std::endl;

    for (int i = 0; i < menuOptions.size(); i++)
    {
        std::cout << i + 1 << ". " << menuOptions.at(i) << std::endl;
    }

    std::cout << "0. Exit" << std::endl;
}