#include <fstream>
#include <iostream>
#include <vector>
#include <sys/stat.h>  // to check if directories exists and stuff
#include <sys/types.h> // for creating directories
// #include <string>
#include <unistd.h>  // to get username
#include <algorithm> // so we can remove \n from end of strings

#include "FileManager.hpp"

FileManager::FileManager(std::string fileName)
{
    this->fileName = fileName;

    createFile();
}

void FileManager::updatePlaylistLink(int playlistNumber)
{
    std::ifstream fPlaylistFile(getFileName()); //gets input from the file

    std::vector<std::string> playlistNames;
    std::vector<std::string> playlistLinks;
    std::string line;
    while (getline(fPlaylistFile, line))
    {
        std::size_t split = line.find('~'); //finds '~'

        // playlists.push_back(line.substr(0, split));
        if ((line.substr(0, split).length() > 1))
        {
            playlistNames.push_back(line.substr(0, split));
            playlistLinks.push_back(line.substr(split + 1, 999));
            // std::cout << playlistNames[0] << std::endl;
        }
    }

    fPlaylistFile.close();

    std::cout << "New link for playlist (cancel to cancel): ";
    std::string newPlaylistLink;
    std::cin >> newPlaylistLink;

    if (compareStrings(newPlaylistLink, "cancel"))
    {
        std::cout << "--- Operation Canceled ---" << std::endl;

        return;
    }

    std::ofstream fPlaylistFileRewrite(getFileName());

    if (!fPlaylistFileRewrite.is_open())
    {
        std::cout << "Couldn't open 'custom-playlists.txt', please check write permissions" << std::endl;
        return;
    }

    std::string newList = "";
    for (int i = 0; i < playlistNames.size(); i++)
    {
        if (i + 1 == playlistNumber)
        {
            playlistLinks[i] = newPlaylistLink;
        }

        newList += playlistNames.at(i) + "~" + playlistLinks.at(i) + "\n";
    }

    fPlaylistFileRewrite << newList;
    fPlaylistFileRewrite.close();

    std::cout << "--- Playlist Updated ---" << std::endl;
}

void FileManager::updatePlaylistName(int playlistNumber)
{
    std::ifstream fPlaylistFile(getFileName()); //gets input from the file

    std::vector<std::string> playlistNames;
    std::vector<std::string> playlistLinks;
    std::string line;
    while (getline(fPlaylistFile, line))
    {
        std::size_t split = line.find('~'); //finds '~'

        // playlists.push_back(line.substr(0, split));
        if ((line.substr(0, split).length() > 1))
        {
            playlistNames.push_back(line.substr(0, split));
            playlistLinks.push_back(line.substr(split + 1, 999));
            // std::cout << playlistNames[0] << std::endl;
        }
    }

    fPlaylistFile.close();

    std::cout << "New name for playlist (cancel to cancel): ";
    std::string newPlaylistName;
    std::cin >> newPlaylistName;

    if (compareStrings(newPlaylistName, "cancel"))
    {
        std::cout << "--- Operation Canceled ---" << std::endl;

        return;
    }

    if (checkPlaylistExists(newPlaylistName))
    {
        updatePlaylistName(playlistNumber);
        return;
    }

    std::ofstream fPlaylistFileRewrite(getFileName());

    if (!fPlaylistFileRewrite.is_open())
    {
        std::cout << "Couldn't open 'custom-playlists.txt', please check write permissions" << std::endl;
        return;
    }

    std::string newList = "";
    for (int i = 0; i < playlistNames.size(); i++)
    {
        if (i + 1 == playlistNumber)
        {
            playlistNames[i] = newPlaylistName;
        }

        newList += playlistNames.at(i) + "~" + playlistLinks.at(i) + "\n";
    }

    fPlaylistFileRewrite << newList;
    fPlaylistFileRewrite.close();

    std::cout << "--- Playlist Updated ---" << std::endl;
}

void FileManager::updatePlaylist()
{
    if (checkFileEmpty())
    {
        std::cout << "No playlists have been added, so there are no playlists to update..." << std::endl;
        return;
    }

    std::vector<std::string> modifyOptions = {"Playlist Name", "Playlist Link"};

    std::cout << "Playlists you have:" << std::endl;
    std::vector<std::string> playlists = getAndShowPlaylists(); // get playlists
    std::cout << "(Playlist to edit) > ";
    int chosenPlaylist = inputSafe(chosenPlaylist);
    if ((chosenPlaylist > playlists.size()) && (chosenPlaylist != 99))
    {
        std::cout << "Error, could not find playlist... Does it exist?" << std::endl;
        updatePlaylist();
        return;
    }
    else if (chosenPlaylist == 99)
    {
        return;
    }

    std::cout << "What would you like to modify?" << std::endl;
    for (int i = 0; i < modifyOptions.size(); i++)
    {
        std::cout << i + 1 << ". " << modifyOptions.at(i) << std::endl;
    }
    std::cout << "99. Cancel" << std::endl;

    std::cout << "(What to edit) > ";

    int chosenOption = inputSafe(chosenOption);

    switch (chosenOption)
    {
    case 1:
        updatePlaylistName(chosenPlaylist);
        break;
    case 2:
        updatePlaylistLink(chosenPlaylist);
        break;
    case 99:
        std::cout << "Playlist not updated" << std::endl;
        return;
        break;
    default:
        std::cout << "Invalid option" << std::endl;
        updatePlaylist();
        break;
    }
}

void FileManager::removePlaylist()
{
    if (checkFileEmpty())
    {
        std::cout << "No playlists have been added, so there are no playlists to remove..." << std::endl;
        return;
    }

    std::cout << "Playlists you have:" << std::endl;

    std::vector<std::string> playlists = getAndShowPlaylists(); // get playlists
    std::cout << "(Playlist to remove) > ";
    int chosenPlaylist = inputSafe(chosenPlaylist);
    if ((chosenPlaylist > playlists.size()) && (chosenPlaylist != 99))
    {
        std::cout << "Error, could not find playlist... Does it exist?" << std::endl;
        removePlaylist();
        return;
    }
    else if (chosenPlaylist == 99)
    {
        return;
    }

    std::cout << "Are you sure you want to delete playlist " << chosenPlaylist << " [y/n]: ";
    std::string confirmDelete;
    std::cin >> confirmDelete;

    if (!compareStrings("y", confirmDelete))
    {
        std::cout << "Playlist not deleted" << std::endl;
        return;
    }

    std::ifstream fPlaylistFile(getFileName()); //gets input from the file

    std::vector<std::string> playlistNames;
    std::vector<std::string> playlistLinks;
    std::string line;
    while (getline(fPlaylistFile, line))
    {
        std::size_t split = line.find('~'); //finds '~'

        // playlists.push_back(line.substr(0, split));
        if ((line.substr(0, split).length() > 1))
        {
            playlistNames.push_back(line.substr(0, split));
            playlistLinks.push_back(line.substr(split + 1, 999));
        }
    }

    fPlaylistFile.close();

    std::ofstream fPlaylistFileRewrite(getFileName());

    if (!fPlaylistFileRewrite.is_open())
    {
        std::cout << "Couldn't open 'custom-playlists.txt', please check write permissions" << std::endl;
        return;
    }

    std::string newList = "";
    for (int i = 0; i < playlistNames.size(); i++)
    {
        if (i + 1 == chosenPlaylist)
        {
            continue;
        }

        newList += playlistNames.at(i) + "~" + playlistLinks.at(i) + "\n";
    }

    fPlaylistFileRewrite << newList;
    fPlaylistFileRewrite.close();

    std::cout << "--- Playlist Deleted ---" << std::endl;
}

std::vector<std::string> FileManager::getAndShowPlaylists()
{
    std::string line;
    std::ifstream fPlaylistFile(getFileName()); //gets input from the file

    std::vector<std::string> playlists;
    std::cout << std::endl;
    int count = 1;
    while (getline(fPlaylistFile, line))
    {
        std::size_t split = line.find('~'); //finds '~'

        // playlists.push_back(line.substr(0, split));
        if ((line.substr(0, split).length() > 1) && (count < 99))
        {
            playlists.push_back(line.substr(split + 1, 999));
            std::cout << count << ". " << line.substr(0, split) << std::endl;
            count++;
        }

        if (count >= 99)
        {
            std::cout << "You have more playlists, but you have reached your limits of 98. Please delete some playlists." << std::endl;
            break;
        }
    }
    std::cout << "99. Cancel" << std::endl;

    fPlaylistFile.close();

    return playlists;
}

void FileManager::playPlaylist()
{
    if (checkFileEmpty())
    {
        std::cout << "No playlists have been added..." << std::endl;
        return;
    }

    std::vector<std::string> playlists = getAndShowPlaylists(); // get links
    std::cout << "(Playlist to play) > ";
    int chosenPlaylist = inputSafe(chosenPlaylist);
    if ((chosenPlaylist > playlists.size()) && (chosenPlaylist != 99))
    {
        std::cout << "Error, could not find playlist... Does it exist?" << std::endl;
        playPlaylist();
        return;
    }
    else if (chosenPlaylist == 99)
    {
        return;
    }
    // std::cout << "sfxnjsdpiojnfiwesnfkjnwdbjhbdx" << std::endl;

    std::cout << "Would you like to shuffle the playlist? [y/n]: ";
    std::string doTheShuffle;
    std::cin >> doTheShuffle;
    // std::cin >> shuffleAns;
    bool shuffle = false;

    if (compareStrings("y", doTheShuffle) || compareStrings("yes", doTheShuffle))
    {
        shuffle = true;
    } // do not delete else if
    else if (compareStrings("n", doTheShuffle) || compareStrings("no", doTheShuffle))
    {
        shuffle = false;
    }
    else
    {
        std::cout << "Invalid answer..." << std::endl;
        playPlaylist();
        return;
    }

    // mpv <link> --no-video --loop-playlist --shuffle
    std::string command = "/usr/bin/bgmplayer_depends/mpv " + playlists[chosenPlaylist - 1] + " --no-video --loop-playlist --script-opts=ytdl_hook-ytdl_path=/usr/bin/bgmplayer_depends/youtube-dl";
    if (shuffle)
    {
        command += " --shuffle";
    }

    std::cout << "Playlist will now start to play (this may take a while to start)... To exit press: q" << std::endl;
    const char *execute = command.c_str();

    system(execute);
}

bool FileManager::checkPlaylistExists(std::string playlistName)
{
    std::string line, playlist;
    std::ifstream fPlaylistFile(getFileName()); //gets input from the file

    while (getline(fPlaylistFile, line))
    {                                       //reads line from file and puts it in variable
        std::size_t split = line.find('~'); //finds ','
        playlist = line.substr(0, split);   //copies username

        if (compareStrings(playlist, playlistName))
        {
            fPlaylistFile.close();
            std::cout << "That playlist already exists (name dulpicate)... Please try again\n"
                      << std::endl;
            return true;
        }
    }

    for (int i = 0; i < playlistName.length(); i++)
    {
        if (playlistName[i] == '~')
        {
            std::cout << "Invalid character found '~'" << std::endl;

            return true;
        }
    }

    fPlaylistFile.close();
    return false;
}

int FileManager::inputSafe(int num)
{
    std::cin >> num;

    while (!std::cin) // make sure input isn't nothing
    {
        std::cout << "\nInvalid value, please retype the correct value: ";
        std::cin.clear();
        std::cin.ignore(100, '\n'); //moves 200 lines then goes to the next line
        std::cin >> num;
    }

    return num;
}

void FileManager::addPlaylist()
{
    std::cout << "Please enter a name for the new playlist(type cancel to cancel): ";
    // std::string playlistName = inputSafe(playlistName);
    std::string playlistName;
    std::cin >> playlistName;

    if (compareStrings(playlistName, "cancel"))
    {
        std::cout << "--- Operation Canceled ---" << std::endl;

        return;
    }

    if (checkPlaylistExists(playlistName))
    {
        addPlaylist();
        return;
    }

    std::cout << "Please enter the link to playlist (type cancel to cancel): ";
    // std::string playlistLink = inputSafe(playlistLink);
    std::string playlistLink;
    std::cin >> playlistLink;

    if (compareStrings(playlistLink, "cancel"))
    {
        std::cout << "--- Operation Canceled ---" << std::endl;

        return;
    }

    std::ofstream fCustomPlaylists;
    fCustomPlaylists.open(getFileName(), std::ios::app | std::ios::out);

    if (!fCustomPlaylists.is_open())
    {
        std::cout << "Couldn't open 'custom-playlists.txt'" << std::endl;
        return;
    }

    std::string newListItem = "\n" + playlistName + "~" + playlistLink;

    fCustomPlaylists << newListItem;

    fCustomPlaylists.close();

    std::cout << "--- Playlist Added ---" << std::endl;
}

bool FileManager::compareStrings(std::string str1, std::string str2)
{
    int str1_length = str1.length();
    int str2_length = str2.length();

    if (str1_length == str2_length)
    {
        for (int i = 0; i < str1_length; i++)
        {
            if (!(str1[i] == str2[i]))
            {
                return false;
            }

            if (str1[i] == '~' || str2[i] == '~')
            {
                std::cout << "'~' is an invalid character" << std::endl;
                return true;
            }
        }

        return true;
    }

    return false;
}

void FileManager::closeFile(std::ofstream fFile)
{
    fFile.close();
}

std::ofstream FileManager::openFile()
{
    std::ofstream fFile(getFileName());

    if (fFile.is_open())
    {
        return fFile;
    }
    std::cout << "File 'custom-playlists' could not open (or be created), please check permissions" << std::endl;

    fFile.close();
    std::_Exit(0);
}

std::string FileManager::getUsernameLinux()
{
    char username[255];
    FILE *name;
    name = popen("whoami", "r");
    fgets(username, sizeof(username), name);
    // std::cout << "Name is : " << username;
    pclose(name);
    std::string uname = username;
    return uname;
}

std::string FileManager::getFileName()
{
    std::string fileName = "/home/" + getUsernameLinux();
    fileName.erase(std::remove(fileName.begin(), fileName.end(), '\n'), fileName.end());
    fileName += "/.ytbgmpcli/" + this->fileName;
    return fileName;
}

bool FileManager::checkFileEmpty() // checks if file is empty
{
    std::ifstream fFile(getFileName());
    bool empty = fFile.peek() == std::ifstream::traits_type::eof(); // true u
    fFile.close();

    return empty;
}

void FileManager::createFile()
{
    std::string location = "/home/" + getUsernameLinux();
    location.erase(std::remove(location.begin(), location.end(), '\n'), location.end());
    location += "/.ytbgmpcli";

    const char *loc = location.c_str();

    struct stat _st = {0};
    if (stat(loc, &_st) == -1) // if location does not exist
    {
        std::cout << "Could not find .ytbgmpcli folder. Creating folder." << std::endl;

        mkdir(loc, 0777);
    }

    struct stat _st2 = {0};

    if (stat(getFileName().c_str(), &_st2) == -1) // if custom links file does not exist
    {
        std::cout << "Could not find customs file. Creating file." << std::endl;

        std::ofstream fFile(getFileName().c_str());
        fFile << "";
        fFile.close();
    }
}

FileManager::~FileManager()
{
}