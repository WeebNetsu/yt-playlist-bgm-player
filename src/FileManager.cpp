#include <fstream>
#include <iostream>
#include <vector>
#include <sys/stat.h>   // to check if directories exists and stuff
#include <sys/types.h>  // for creating directories
#include <unistd.h>     // to get username
#include <algorithm>    // so we can remove \n from end of strings
#include <regex>        // used when working with local files
// #include <mpv/client.h> // to use mpv without system()
#include <map>

#include "../include/FileManager.hpp"
#include "../deps/CommonMethods.hpp"

// Constructor
FileManager::FileManager(std::string fileName)
{
    this->fileName = fileName;

    // the below code will setup everything if this is the first time the application is being run
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

// returns all the playlists but does not out to the terminal
std::vector<std::string> FileManager::getPlaylistsDontShow()
{
    std::string line;
    std::ifstream fPlaylistFile(getFileName()); //gets input from the file

    std::vector<std::string> playlists;
    int count = 1;
    while (getline(fPlaylistFile, line))
    {
        bool local = false;
        std::size_t split = line.find('~'); //finds '~'

        // if ~ was not found, then the playlist is a local playlist
        if (split == -1)
        {
            split = line.find('`');
            local = true;
        }

        if (line.substr(0, split).length() > 1)
        {
            // if place playlists in array (vector) and add a "/" before the playlist if it's local
            local ? playlists.push_back("/" + line.substr(split + 1, 999)) : playlists.push_back(line.substr(split + 1, 999));

            count++;
        }
    }

    fPlaylistFile.close();

    return playlists;
}

// if the user wants to play playlists without opening the interface
void FileManager::instantPlayPlaylists(std::vector<int> playlistsToPlay, std::map<std::string, bool> flags/* , bool shuffle, bool loop */)
{
    // std::cout << "HERE" << std::endl;

    // if the file is empty
    if (checkFileEmpty())
    {
        std::cout << "No playlists have been added..." << std::endl;
        return;
    }

    std::vector<std::string> playlists = getPlaylistsDontShow(); // get links without displaying it to the user

    // cleanlist is basically the playlists vector but modified to be playable
    // playlists vector: "/location/to the/folder"
    // cleanList vector: "/location/to\ the/folder/*"
    std::vector<std::string> cleanList;
    for (int playlistNum : playlistsToPlay)
    {
        /*
            if the playlist number is larger than the amount of links in the playlist
            since playlists are counted we can make sure that if 10 was counted and the user
            entered 11 then we know the playlist did not exitst (note: we start counting at 1, not 0)
        */
        if (playlistNum > playlists.size())
        {
            std::cout << "Error, could not find playlist nr: " << playlistNum << "... Does it exist?" << std::endl;
            return;
        }

        // playlistNum -1 since playlist 2 will be at index 1
        std::string playlist = playlists[playlistNum - 1];

        while (playlist.size())
        {
            //the number 33 refers to ascii character 33... the first printable character
            if (playlist[0] < 33)
            {
                playlist.erase(playlist.begin()); //remove all leading whitespace
            }
            else if (playlist[playlist.size() - 1] < 33)
            {
                playlist.erase(playlist.size() - 1); //remove all trailing whitespace
            }
            else
            {
                if (playlist[0] == '/')
                {
                    std::regex rep(" "); // what should be found
                    // replace all spaces with "\\ "
                    playlist = regex_replace(playlist, rep, "\\ ");
                    cleanList.push_back(playlist.append("/*"));
                }
                else
                {
                    cleanList.push_back(playlist);
                }
                break;
            }
        }
    }

    // the command that will be given to the terminal
    std::string command;

    // basically all the playlists but seperated by spaces
    std::string playlistsCommand = "";
    for (std::string playlist : cleanList)
    {
        playlistsCommand += playlist + " ";
    }

    // basic mpv command: mpv playlist --no-video youtube-dl-location
    command = "mpv " + playlistsCommand + " --no-video --script-opts=ytdl_hook-ytdl_path=/usr/local/bin/youtube-dlc";
    if (flags["shuffle"]) // if the user wants to shuffle
    {
        command += " --shuffle";
    }

    if (flags["loop"]) // if the user wants to loop the playlists
    {
        command += " --loop-playlist";
    }

    // terminal can't read std::string, so convert to char
    const char *execute = command.c_str();

    // display the player controls to the user
    displayPlayerControls();

    // execute!
    system(execute);
}

std::string FileManager::rewritePlaylists(int playlistNumber, ModType modType, std::string newChange)
{
    std::vector<std::string> playlistNames;
    std::vector<std::string> playlistLinks;
    std::vector<bool> local;

    std::ifstream fPlaylistFile(getFileName()); //gets input from the file

    std::string line;
    while (getline(fPlaylistFile, line))
    {
        std::size_t split = line.find('~');

        // if ~ not found, then playlist is local
        if (split == -1)
        {
            split = line.find('`');
            local.push_back(true);
        }
        else
        {
            local.push_back(false);
        }

        // if there is a playlist on the line
        if ((line.substr(0, split).length() > 1))
        {
            playlistNames.push_back(line.substr(0, split));
            playlistLinks.push_back(line.substr(split + 1, 999));
        }
    }

    fPlaylistFile.close();

    std::string newList = "";
    for (int i = 0; i < playlistNames.size(); i++)
    {
        // if the playlistNumber we want to edit and i+1 is the same
        if (i + 1 == playlistNumber)
        {
            switch (modType)
            {
            case NAME:
                playlistNames[i] = newChange;
                break;
            case LINK:
                playlistLinks[i] = newChange;
                break;
            case LOCAL:
                local[i] = !local[i];
                break;
            case DELETE:
                // if the playlist item should be deleted, just don't add it to the newList
                // by skipping the loop
                continue;
            default:
                break;
            }
        }

        // check if playlist is stored locally
        if (local.at(i))
        {
            newList += playlistNames.at(i) + "`" + playlistLinks.at(i) + "\n";
        }
        else
        {
            newList += playlistNames.at(i) + "~" + playlistLinks.at(i) + "\n";
        }
    }

    return newList;
}

void FileManager::rewriteFile(std::string newList)
{
    std::ofstream fPlaylistFileRewrite(getFileName());

    if (!fPlaylistFileRewrite.is_open())
    {
        std::cout << "Couldn't open 'custom-playlists.txt', please check write permissions" << std::endl;
        return;
    }

    fPlaylistFileRewrite << newList; // rewrite playlist file
    fPlaylistFileRewrite.close();

    std::cout << "--- Playlist Updated ---" << std::endl;
}

// edit the link a playlist has
void FileManager::updatePlaylistLink(int playlistNumber)
{
    std::cout << "New link/location for playlist (cancel to cancel): ";
    std::string newPlaylistLink;

    cmds::clearInput();
    std::getline(std::cin, newPlaylistLink);

    // if the user canceled the operation
    if (cmds::compareStrings(newPlaylistLink, "cancel"))
    {
        std::cout << "--- Operation Canceled ---" << std::endl;
        return;
    }
    else if (cmds::compareStrings(newPlaylistLink, ""))
    {
        std::cout << "Empty line found. Operation Canceled." << std::endl;
        return;
    }

    std::string newList = rewritePlaylists(playlistNumber, LINK, newPlaylistLink);

    rewriteFile(newList);
}

// edit the name of a playlist
void FileManager::updatePlaylistName(int playlistNumber)
{
    std::cout << "New name for playlist (cancel to cancel): ";
    std::string newPlaylistName;

    cmds::clearInput();
    std::getline(std::cin, newPlaylistName);

    if (cmds::compareStrings(newPlaylistName, "cancel"))
    {
        std::cout << "--- Operation Canceled ---" << std::endl;
        return;
    }
    else if (cmds::compareStrings(newPlaylistName, ""))
    {
        std::cout << "Empty line found. Operation Canceled." << std::endl;
        return;
    }

    // if the playlist name already exists
    if (checkPlaylistExists(newPlaylistName))
    {
        return;
    }

    std::string newList = rewritePlaylists(playlistNumber, NAME, newPlaylistName);

    rewriteFile(newList);
}

// allows you to edit playlist details
void FileManager::updatePlaylist()
{
    // if there are no playlists to edit
    if (checkFileEmpty())
    {
        std::cout << "No playlists have been added, so there are no playlists to update..." << std::endl;
        return;
    }

    std::cout << "Playlists you have:" << std::endl;
    std::vector<std::string> playlists = getAndShowPlaylists(); // get playlists and display it in the terminal

    // choose a playlist to edit
    std::cout << "(Playlist to edit) > ";
    int chosenPlaylist = cmds::numberInput();

    // if the playlist does not exist
    if ((chosenPlaylist > playlists.size()) && (chosenPlaylist != 0))
    {
        std::cout << "Error, could not find playlist... Does it exist?" << std::endl;
        updatePlaylist();
        return;
    }
    else if (chosenPlaylist == 0)
    {
        return;
    }

    // these are the options you can change of the playlist
    std::vector<std::string> modifyOptions = {"Playlist Name", "Playlist Link/Location", "Local/Online"};

    std::cout << "What would you like to modify?" << std::endl;
    for (int i = 0; i < modifyOptions.size(); i++)
    { // list out options
        std::cout << i + 1 << ". " << modifyOptions.at(i) << std::endl;
    }

    std::cout << "0. Cancel" << std::endl;

    std::cout << "(What to edit) > ";

    int chosenOption = cmds::numberInput();

    switch (chosenOption)
    {
    case 1:
        updatePlaylistName(chosenPlaylist);
        break;
    case 2:
        updatePlaylistLink(chosenPlaylist);
        break;
    case 3:
        // change playlist status from online -> local and vice versa
        rewriteFile(rewritePlaylists(chosenPlaylist, LOCAL, ""));
        break;
    case 0:
        std::cout << "Playlist not updated" << std::endl;
        return;
        break;
    default:
        std::cout << "Invalid option" << std::endl;
        updatePlaylist();
        break;
    }
}

// delete a playlist
void FileManager::removePlaylist()
{
    // make sure threre is a playlist to delete
    if (checkFileEmpty())
    {
        std::cout << "No playlists have been added, so there are no playlists to remove..." << std::endl;
        return;
    }

    std::cout << "Playlists you have:" << std::endl;
    std::vector<std::string> playlists = getAndShowPlaylists(); // get playlists

    std::cout << "(Playlist to remove) > ";
    int chosenPlaylist = cmds::numberInput();

    // if the playlist they chose does not exists
    if ((chosenPlaylist > playlists.size()) && (chosenPlaylist != 0))
    {
        std::cout << "Error, could not find playlist... Does it exist?" << std::endl;
        removePlaylist();
        return;
    }
    else if (chosenPlaylist == 0)
    {
        return;
    }

    // make sure they really intend to delete the playlist
    std::cout << "Are you sure you want to delete playlist " << chosenPlaylist << " [y/n]: ";
    std::string confirmDelete;
    std::cin >> confirmDelete;

    // if they don't want to delete the playlist
    if (!cmds::compareStrings("y", confirmDelete))
    {
        std::cout << "Playlist not deleted" << std::endl;
        return;
    }

    std::string newList = rewritePlaylists(chosenPlaylist, DELETE, "");
    rewriteFile(newList);
}

// print out playlists to termina
void FileManager::showPlaylists()
{
    std::string line;
    std::ifstream fPlaylistFile(getFileName()); //gets input from the file

    int count = 1;
    while (getline(fPlaylistFile, line))
    {
        std::size_t split = line.find('~'); //finds '~'

        if (split == -1)
        {
            split = line.find('`');
        }

        if ((line.substr(0, split).length() > 1))
        {
            // print out like this: 1. playlist name
            std::cout << count << ". " << line.substr(0, split) << std::endl;
            count++;
        }
    }

    fPlaylistFile.close();
}

// returns the playlists as well as displaying it in the terminal
std::vector<std::string> FileManager::getAndShowPlaylists()
{
    std::string line;
    std::ifstream fPlaylistFile(getFileName()); //gets input from the file

    std::vector<std::string> playlists;
    std::cout << std::endl;
    int count = 1;
    while (getline(fPlaylistFile, line))
    {
        bool local = false;
        std::size_t split = line.find('~'); //finds '~'

        if (split == -1)
        {
            split = line.find('`');
            local = true;
        }

        if ((line.substr(0, split).length() > 1))
        {
            if (local)
            {
                // if the playlist is stored locally, it will start with a '/'
                // a local location starting with //home/whatever will not cause any errors
                // in a linux enviroment and will start from root
                playlists.push_back("/" + line.substr(split + 1, 999));
            }
            else
            {
                playlists.push_back(line.substr(split + 1, 999));
            }

            std::cout << count << ". " << line.substr(0, split) << std::endl;
            count++;
        }
    }
    std::cout << "0. Cancel" << std::endl;

    fPlaylistFile.close();

    return playlists;
}

// prints out the player (mpv) controls to the terminal
void FileManager::displayPlayerControls()
{
    std::cout << "\nPlayer Controls:" << std::endl;
    std::cout << "\t[SPACE] - Pause\t\t\tm - Mute" << std::endl;
    std::cout << "\t[Right Arrow] - Skip 10 Seconds\t[Left Arrow] - Rewind 10 Seconds" << std::endl;
    std::cout << "\t[Up Arrow] - Skip 1 Minute\t[Down Arrow] - Rewind 1 Minute" << std::endl;
    std::cout << "\t> - Next Song\t\t\t< - Previous Song" << std::endl;
    std::cout << "\tq - Quit" << std::endl;
}

// gets if the user wants to shuffle the playlist
bool FileManager::shufflePlaylist()
{
    std::cout << "Would you like to shuffle the playlists? [y/n]: ";
    std::string doTheShuffle;
    std::cin >> doTheShuffle;

    if (cmds::compareStrings("y", doTheShuffle) || cmds::compareStrings("yes", doTheShuffle))
    {
        return true;
    } // do not delete else if
    else if (cmds::compareStrings("n", doTheShuffle) || cmds::compareStrings("no", doTheShuffle))
    {
        return false;
    }
    else
    {
        std::cout << "Invalid option..." << std::endl;
        return shufflePlaylist();
    }
}

// if you want to play multiple playlists at once
void FileManager::playPlaylists()
{
    // if the playlist file is empty
    if (checkFileEmpty())
    {
        std::cout << "No playlists have been added..." << std::endl;
        return;
    }

    std::vector<std::string> playlists = getAndShowPlaylists(); // get links
    std::cout << "(Playlists to play [separated by spaces]) > ";
    std::string itemsToPlay;

    cmds::clearInput();
    std::getline(std::cin, itemsToPlay);

    // remove white spaces before and after string
    itemsToPlay = cmds::skinStr(itemsToPlay);

    // nothing was entered
    if (itemsToPlay.length() < 1)
    {
        std::cout << "No playlist(s) found." << std::endl;
        return;
    }

    // add a space to the end of the string
    itemsToPlay += " ";

    std::vector<int> lists;
    std::string num;
    // add playlists to play to lists array
    for (char i : itemsToPlay)
    {
        if (i == ' ')
        {
            std::regex rep("^[0-9]+$");
            // if letters were found inide the input
            if (!regex_match(num, rep))
            {
                std::cout << "Only numbers allowed in the input." << std::endl;

                return;
            }

            // add playlist number to play to lists
            lists.push_back(std::stoi(num));

            num = "";
        }
        else
        {
            // add character to num
            num += i;
        }
    }

    // clean list is the list but fixed to be a good input for mpv
    std::vector<std::string> cleanList;
    for (int playlistNum : lists)
    {
        // check if the playlist exists and if they want to cancel
        if ((playlistNum > playlists.size()) && (playlistNum != 0))
        {
            std::cout << "Error, could not find playlist nr: " << playlistNum << "... Does it exist?" << std::endl;
            playPlaylists();
            return;
        }
        else if (playlistNum == 0)
        {
            std::cout << "Canceled." << std::endl;
            return;
        }

        std::string playlist = playlists[playlistNum - 1];

        while (playlist.size())
        {
            //the number 33 refers to ascii character 33... the first printable character
            if (playlist[0] < 33)
            {
                playlist.erase(playlist.begin()); //remove all leading whitespace
            }
            else if (playlist[playlist.size() - 1] < 33)
            {
                playlist.erase(playlist.size() - 1); //remove all trailing whitespace
            }
            else
            {
                if (playlist[0] == '/')
                {
                    std::regex rep(" ");
                    playlist = regex_replace(playlist, rep, "\\ ");
                    cleanList.push_back(playlist.append("/*"));
                }
                else
                {
                    cleanList.push_back(playlist);
                }
                break;
            }
        }
    }

    bool shuffle = shufflePlaylist();

    std::string command; // this will be entered into the console

    std::string playlistsCommand = ""; // all the playlists that should be played
    for (std::string playlist : cleanList)
    {
        playlistsCommand += playlist + " ";
    }

    cmds::clearInput();

    // check if the user wants to loop the playlists
    std::string loop;
    std::cout << "Loop playlists? [y/n]: ";
    std::cin >> loop;

    // if they want to loop
    if (cmds::compareStrings("y", loop) || cmds::compareStrings("yes", loop))
    {
        command = "mpv " + playlistsCommand + " --no-video --loop-playlist --script-opts=ytdl_hook-ytdl_path=/usr/local/bin/youtube-dlc";
    } // do not delete else if
    else if (cmds::compareStrings("n", loop) || cmds::compareStrings("no", loop))
    {
        command = "mpv " + playlistsCommand + " --no-video --script-opts=ytdl_hook-ytdl_path=/usr/local/bin/youtube-dlc";
    }
    else
    {
        std::cout << "Invalid option..." << std::endl;
        playPlaylists();
        return;
    }

    // check if they want to shuffle
    if (shuffle)
    {
        command += " --shuffle";
    }

    // since the terminal cant read strings, transform into character
    const char *execute = command.c_str();

    displayPlayerControls();

    system(execute);
}

// search if the playlist name already exists in the file
bool FileManager::checkPlaylistExists(std::string playlistName)
{
    std::string line, playlist;
    std::ifstream fPlaylistFile(getFileName()); //gets input from the file

    while (getline(fPlaylistFile, line))
    {
        std::size_t split = line.find('~');
        if (split == -1) // if the playlist is stored locally
        {
            split = line.find('`');
        }
        playlist = line.substr(0, split); //copies playlist name

        // check if the playlist already exitsts
        if (cmds::compareStrings(playlist, playlistName))
        {
            fPlaylistFile.close();
            std::cout << "That playlist already exists (name dulpicate)...\n"
                      << std::endl;
            return true;
        }
    }

    fPlaylistFile.close();

    // makes sure that the user doesn't add a ~ or ` in the name of the playlist
    for (int i = 0; i < playlistName.length(); i++)
    {
        if (playlistName[i] == '~')
        {
            std::cout << "Invalid character found '~'" << std::endl;

            return true;
        }
        else if (playlistName[i] == '`')
        {
            std::cout << "Invalid character found '`'" << std::endl;

            return true;
        }
    }

    return false;
}

// add a playlist to list of playlist
void FileManager::addPlaylist()
{
    std::cout << "Please enter a name for the new playlist(type cancel to cancel): ";
    std::string playlistName;
    cmds::clearInput();
    std::getline(std::cin, playlistName);

    // check if user wants to cancel
    if (cmds::compareStrings(playlistName, "cancel"))
    {
        std::cout << "--- Operation Canceled ---" << std::endl;

        return;
    }

    // make sure there is no playlist with the same name
    if (checkPlaylistExists(playlistName))
    {
        addPlaylist();
        return;
    }

    // find out if playlist is on youtube or on the computer
    std::cout << "Where is your playlist? " << std::endl;
    std::cout << "1. On my machine (local) " << std::endl;
    std::cout << "2. On YouTube (online) " << std::endl;
    std::cout << "0. Cancel " << std::endl;
    std::cout << "> ";
    int location;
    std::cin >> location;

    std::ofstream fCustomPlaylists;
    fCustomPlaylists.open(getFileName(), std::ios::app | std::ios::out);

    if (!fCustomPlaylists.is_open())
    {
        std::cout << "Couldn't open 'custom-playlists.txt'" << std::endl;
        return;
    }

    // if playlist is stored locally
    if (location == 1)
    {
        std::cout << "You're about to add a location. This is how your location should look: /home/netsu/my music\n";
        std::cout << "To get the above results, you can type pwd (linux) to get the location to a directory you're in\n";
        std::cout << "Please do not add any '\\', '\"' or '\'' to the location.\n"
                  << std::endl;
        std::cout << "Please enter the location of the folder (from root(/)) (cancel to cancel): ";
        std::string playlistLocation;
        cmds::clearInput();
        std::getline(std::cin, playlistLocation);

        // make sure they didn't cancel
        if (cmds::compareStrings(playlistLocation, "cancel"))
        {
            std::cout << "--- Operation Canceled ---" << std::endl;
            fCustomPlaylists.close();

            return;
        }

        // I can't remember why I restrict it, it's probably because of mpv
        if (playlistLocation.length() > 60)
        {
            std::cout << "Path to playlist is too long. Try moving it to a folder closer to root (/)" << std::endl;
            fCustomPlaylists.close();

            return;
        }

        std::string newListItem = "\n" + playlistName + "`" + playlistLocation;

        fCustomPlaylists << newListItem;
    }
    else if (location == 2) // if stored on youtube
    {
        std::cout << "Please enter the link to playlist (type cancel to cancel): ";
        std::string playlistLink;
        std::cin >> playlistLink;

        if (cmds::compareStrings(playlistLink, "cancel"))
        {
            std::cout << "--- Operation Canceled ---" << std::endl;
            fCustomPlaylists.close();

            return;
        }

        std::string newListItem = "\n" + playlistName + "~" + playlistLink;

        fCustomPlaylists << newListItem;
    }
    else // if anything other than 1 or 2 was entered
    {
        fCustomPlaylists.close();
        std::cout << "Cancel" << std::endl;

        return;
    }

    fCustomPlaylists.close();
    std::cout << "--- Playlist Added ---" << std::endl;
}

// gets the users username
std::string FileManager::getUsernameLinux()
{
    char username[255];
    FILE *name;
    name = popen("whoami", "r");
    fgets(username, sizeof(username), name);
    pclose(name);
    std::string uname = username;
    return uname;
}

// return the name/location of the file where the playlists are stored
std::string FileManager::getFileName()
{
    std::string fileName = "/home/" + getUsernameLinux();
    fileName.erase(std::remove(fileName.begin(), fileName.end(), '\n'), fileName.end());
    fileName += "/.ytbgmpcli/" + this->fileName;
    return fileName;
}

// this does the same as getFileName, but for a custom file and not the playlists file
std::string FileManager::getFileName(std::string tempFileName)
{
    std::string fileName = "/home/" + getUsernameLinux();
    fileName.erase(std::remove(fileName.begin(), fileName.end(), '\n'), fileName.end());
    fileName += "/.ytbgmpcli/" + tempFileName;
    return fileName;
}

// checks if file is empty
bool FileManager::checkFileEmpty()
{
    std::ifstream fFile(getFileName());
    bool empty = fFile.peek() == std::ifstream::traits_type::eof(); // true u
    fFile.close();

    return empty;
}

// Destructor
FileManager::~FileManager()
{
}