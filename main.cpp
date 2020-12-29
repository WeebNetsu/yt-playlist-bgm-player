#include <iostream>
#include <fstream>
#include <vector>

#include "FileManager.hpp"

void displayMenu(std::vector<std::string> menuOptions);
void processAnswer(int option, std::vector<std::string> menuOptions, FileManager *fm);
void displayHelp();
bool running = true;

int main(int argc, char const *argv[])
{
    std::vector<std::string> menuOptions = {"Play Playlist", "Add Playlist", "Remove Playlist", "Edit Playlist", "Help"};

    std::cout << "Welcome!" << std::endl;

    FileManager fm("custom-playlists.txt");

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
        displayHelp();
        std::cout << std::endl;
        break;

    case 99:
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

void displayHelp()
{
    std::cout << "Welcome!" << std::endl;
    std::cout << "This CLI application is intended to help you play music in the background from YouTube." << std::endl;
    std::cout << "Now, let's look at the menu and talk about it:\n"
              << std::endl;
    std::cout << "\tPlay Playlist: This option will read from the playlists saved inside\n\t'custom-playlists.txt' and display the options to you. You can choose a\n\tplaylist, and if everyting has been inserted correctly and the playlist still\n\texists, it will play the playlist (looping the playlist)\n"
              << std::endl;
    std::cout << "\tAdd Playlist: If you want to add a new playlist to your list (please limit it to\n\t98 playlists). You can insert a playlist name which should NOT contain any '~'.\n\tThen you can insert a playlist link copied from YouTube. The\n\tplaylist should be added to the file with no problem.\n"
              << std::endl;
    std::cout << "\tRemove Playlist: If there is a playlist you don't like using, just remove it\n\tfrom the list. If you want to listen to it after deleting it, you'll have to add\n\tit again.\n"
              << std::endl;
    std::cout << "\tEdit Playlist: Did you perhaps insert the wrong name or link for your playlist?\n\tOr did the link change? This option will allow you to edit the playlist name and\n\tlink.\n"
              << std::endl;
    std::cout << "\tHelp: Display some help you may need.\n"
              << std::endl;

    std::cout << "Some problems may still happen, so if it does, edit the code yourself if you have the\nsource code or remove the problem from the 'custom-playlists.txt' file(if the problem\nis from the playlists file), or contact me on Github: https://github.com/WeebNetsu" << std::endl;
}

void displayMenu(std::vector<std::string> menuOptions)
{
    std::cout << "What would you like to do?" << std::endl;

    for (int i = 0; i < menuOptions.size(); i++)
    {
        std::cout << i + 1 << ". " << menuOptions.at(i) << std::endl;
    }

    std::cout << "99. Exit" << std::endl;
}