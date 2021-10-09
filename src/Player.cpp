#include <fstream>
#include <iostream>
#include <vector>
#include <sys/stat.h>  // to check if directories exists and stuff
#include <sys/types.h> // for creating directories
#include <unistd.h>    // to get username
#include <algorithm>   // so we can remove \n from end of strings
#include <regex>       // used when working with local files
// #include <mpv/client.h> // to use mpv without system()
#include <map>

#include "../include/Player.hpp"
#include "../include/CommonMethods.hpp"

// create a new bgmplayer, with a custom file name
Player::Player(std::string fileName)
{
    this->fileName = fileName;

    // the below will setup everything if you don't have the playlists file in ~/.ytbgmpcli/
    setup();
}

// returns all the playlists but does not output to the terminal
std::vector<std::string> Player::getPlaylistsDontShow()
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
void Player::instantPlayPlaylists(std::vector<int> playlistsToPlay, std::map<std::string, bool> flags)
{
    // if the file is empty
    if (checkFileEmpty())
    {
        cmds::showMessage("No playlists have been added. Playlist file is empty.", "error");
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
            cmds::showMessage("Could not find playlist nr: " + std::to_string(playlistNum) + "... Does it exist?", "error");
            return;
        }

        // playlistNum - 1 since playlist 2 will be at index 1
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
    command = "mpv " + playlistsCommand + " --no-video --script-opts=ytdl_hook-ytdl_path=$HOME/bin/youtube-dlc";
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

    cmds::showMessage("Goodbye!", "notice");
}

// edit a playlist (returns result)
std::string Player::rewritePlaylists(int playlistNumber, ModType modType, std::string newChange)
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

// will setup the playlist files if the user doesn't have one yet
void Player::setup()
{
    // the below code will setup everything if this is the first time the application is being run
    std::string location = "/home/" + getUsernameLinux();
    location.erase(std::remove(location.begin(), location.end(), '\n'), location.end());
    location += "/.ytbgmpcli";

    const char *loc = location.c_str();

    struct stat _st = {0};
    if (stat(loc, &_st) == -1) // if location does not exist
    {
        cmds::showMessage("Could not find .ytbgmpcli folder. Creating folder.", "notice");

        mkdir(loc, 0777);
    }

    struct stat _st2 = {0};

    if (stat(getFileName().c_str(), &_st2) == -1) // if custom links file does not exist
    {
        cmds::showMessage("Could not find customs file. Creating file.", "notice");

        std::ofstream fFile(getFileName().c_str());
        fFile << "";
        fFile.close();
    }
}

// edit playlist file (modifies file)
void Player::rewriteFile(std::string newList)
{
    std::ofstream fPlaylistFileRewrite(getFileName());

    if (!fPlaylistFileRewrite.is_open())
    {
        cmds::showMessage("Couldn't open 'custom-playlists.txt', please check write permissions", "error");
        return;
    }

    fPlaylistFileRewrite << newList; // rewrite playlist file
    fPlaylistFileRewrite.close();

    cmds::showMessage("--- Playlist Updated ---", "success");
}

// edit the link a playlist has
void Player::updatePlaylistLink(int playlistNumber)
{
    std::cout << "New link/location for playlist (cancel to cancel): ";
    std::string newPlaylistLink;

    cmds::clearInput();
    std::getline(std::cin, newPlaylistLink);

    // if the user canceled the operation
    if (cmds::compareStrings(newPlaylistLink, "cancel"))
    {
        cmds::showMessage("--- Operation Canceled ---", "notice");
        return;
    }
    else if (cmds::compareStrings(newPlaylistLink, ""))
    {
        cmds::showMessage("Empty line found. Operation Canceled.", "warning");
        return;
    }

    std::string newList = rewritePlaylists(playlistNumber, LINK, newPlaylistLink);

    rewriteFile(newList);
}

// edit the name of a playlist
void Player::updatePlaylistName(int playlistNumber)
{
    std::cout << "New name for playlist (cancel to cancel): ";
    std::string newPlaylistName;

    cmds::clearInput();
    std::getline(std::cin, newPlaylistName);

    if (cmds::compareStrings(newPlaylistName, "cancel"))
    {
        cmds::showMessage("--- Operation Canceled ---", "notice");
        return;
    }
    else if (cmds::compareStrings(newPlaylistName, ""))
    {
        cmds::showMessage("Empty line found. Operation Canceled.", "warning");
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
void Player::updatePlaylist()
{
    // if there are no playlists to edit
    if (checkFileEmpty())
    {
        cmds::showMessage("No playlists have been added, so there are no playlists to update...", "notice");
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
        cmds::showMessage("Could not find playlist... Does it exist?", "warning");
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
        cmds::showMessage("Playlist not updated", "warning");
        return;
        break;
    default:
        cmds::showMessage("Invalid option", "warning");
        updatePlaylist();
        break;
    }
}

// delete a playlist
void Player::removePlaylist()
{
    // make sure threre is a playlist to delete
    if (checkFileEmpty())
    {
        cmds::showMessage("No playlists have been added, so there are no playlists to remove...", "notice");
        return;
    }

    std::cout << "Playlists you have:" << std::endl;
    std::vector<std::string> playlists = getAndShowPlaylists(); // get playlists

    std::cout << "(Playlist to remove) > ";
    int chosenPlaylist = cmds::numberInput();

    // if the playlist they chose does not exists
    if ((chosenPlaylist > playlists.size()) && (chosenPlaylist != 0))
    {
        cmds::showMessage("Could not find playlist... Does it exist?", "warning");
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
        cmds::showMessage("Playlist not deleted", "notice");
        return;
    }

    std::string newList = rewritePlaylists(chosenPlaylist, DELETE, "");
    rewriteFile(newList);
    cmds::showMessage("Playlist deleted", "warning");
}

// print out playlists to terminal
void Player::showPlaylists()
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
std::vector<std::string> Player::getAndShowPlaylists()
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
void Player::displayPlayerControls()
{
    std::cout << "\nPlayer Controls:" << std::endl;
    std::cout << "\t[SPACE] - Pause\t\t\tm - Mute" << std::endl;
    std::cout << "\t[Right Arrow] - Skip 10 Seconds\t[Left Arrow] - Rewind 10 Seconds" << std::endl;
    std::cout << "\t[Up Arrow] - Skip 1 Minute\t[Down Arrow] - Rewind 1 Minute" << std::endl;
    std::cout << "\t> - Next Song\t\t\t< - Previous Song" << std::endl;
    std::cout << "\tq - Quit" << std::endl;
}

// gets if the user wants to shuffle the playlist
bool Player::shufflePlaylist()
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
        cmds::showMessage("Invalid option...", "warning");
        return shufflePlaylist();
    }
}

// if you want to play multiple playlists at once
void Player::playPlaylists()
{
    // if the playlist file is empty
    if (checkFileEmpty())
    {
        cmds::showMessage("No playlists have been added, playlist file is empty.", "warning");
        return;
    }

    std::vector<std::string> playlists = getAndShowPlaylists(); // get links
    std::cout << "(Playlists to play [separated by spaces]) > ";
    std::string itemsToPlay;

    cmds::clearInput();
    std::getline(std::cin, itemsToPlay);

    // remove white spaces before and after string
    itemsToPlay = cmds::trimStr(itemsToPlay);

    // nothing was entered
    if (itemsToPlay.length() < 1)
    {
        cmds::showMessage("No playlist chosen. Canceled.", "warning");
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
                cmds::showMessage("Only numbers allowed in the input.", "warning");

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
            cmds::showMessage("Error, could not find playlist nr: " + std::to_string(playlistNum) + "... Does it exist?", "warning");
            playPlaylists();
            return;
        }
        else if (playlistNum == 0)
        {
            cmds::showMessage("Canceled.", "warning");
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
        command = "mpv " + playlistsCommand + " --no-video --loop-playlist --script-opts=ytdl_hook-ytdl_path=$HOME/bin/youtube-dlc";
    } // do not delete else if
    else if (cmds::compareStrings("n", loop) || cmds::compareStrings("no", loop))
    {
        command = "mpv " + playlistsCommand + " --no-video --script-opts=ytdl_hook-ytdl_path=$HOME/bin/youtube-dlc";
    }
    else
    {
        cmds::showMessage("Invalid option...", "warning");
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
bool Player::checkPlaylistExists(std::string playlistName)
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
            cmds::showMessage("That playlist already exists (name dulpicate)...\n", "warning");
            return true;
        }
    }

    fPlaylistFile.close();

    // makes sure that the user doesn't add a ~ or ` in the name of the playlist
    for (int i = 0; i < playlistName.length(); i++)
    {
        if ((playlistName[i] == '~') || (playlistName[i] == '`'))
        {
            cmds::showMessage("Invalid character found '~' or '`'", "warning");

            return true;
        }
    }

    return false;
}

// add a playlist to list of playlist
void Player::addPlaylist()
{
    std::cout << "Please enter a name for the new playlist(type cancel to cancel): ";
    std::string playlistName;
    cmds::clearInput();
    std::getline(std::cin, playlistName);

    // check if user wants to cancel
    if (cmds::compareStrings(playlistName, "cancel"))
    {
        cmds::showMessage("--- Operation Canceled ---", "notice");

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
        cmds::showMessage("Couldn't open 'custom-playlists.txt', check read & write permissions.", "warning");
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
            cmds::showMessage("Path to playlist is too long. Try moving it to a folder closer to root (/)", "warning");
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
            cmds::showMessage("--- Operation Canceled ---", "notice");
            fCustomPlaylists.close();

            return;
        }

        std::string newListItem = "\n" + playlistName + "~" + playlistLink;

        fCustomPlaylists << newListItem;
    }
    else // if anything other than 1 or 2 was entered
    {
        fCustomPlaylists.close();
        cmds::showMessage("Cancel", "notice");

        return;
    }

    fCustomPlaylists.close();
    cmds::showMessage("--- Playlist Added ---", "success");
}

// gets the users username
std::string Player::getUsernameLinux()
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
std::string Player::getFileName()
{
    std::string fileName = "/home/" + getUsernameLinux();
    fileName.erase(std::remove(fileName.begin(), fileName.end(), '\n'), fileName.end());
    fileName += "/.ytbgmpcli/" + this->fileName;
    return fileName;
}

// this does the same as getFileName, but for a custom file and not the playlists file
std::string Player::getFileName(std::string tempFileName)
{
    std::string fileName = "/home/" + getUsernameLinux();
    fileName.erase(std::remove(fileName.begin(), fileName.end(), '\n'), fileName.end());
    fileName += "/.ytbgmpcli/" + tempFileName;
    return fileName;
}

// checks if file is empty
bool Player::checkFileEmpty()
{
    std::ifstream fFile(getFileName());
    bool empty = fFile.peek() == std::ifstream::traits_type::eof(); // true u
    fFile.close();

    return empty;
}

// Destructor
Player::~Player()
{
}