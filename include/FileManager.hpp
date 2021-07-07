#if !defined(FILE_MANAGER)
#define FILE_MANAGER

#include <iostream>
#include <fstream>
#include <map>

class FileManager
{
private:
    std::string fileName;
    enum ModType
    {
        NAME,
        LINK,
        LOCAL,
        DELETE
    };

public:
    FileManager(std::string fileName);
    ~FileManager();

    std::string getFileName();
    std::string getFileName(std::string tempFileName);

    bool checkFileEmpty();

    void addPlaylist();
    void playPlaylists();
    void removePlaylist();
    void updatePlaylist();
    void instantPlayPlaylists(std::vector<int> playlistsToPlay, std::map<std::string, bool> flags);

    void updatePlaylistName(int playlistNumber);
    void updatePlaylistLink(int playlistNumber);

    std::string rewritePlaylists(int playlistNumber, ModType modType, std::string newChange);
    void rewriteFile(std::string newList);

    std::vector<std::string> getAndShowPlaylists();
    void showPlaylists();
    std::vector<std::string> getPlaylistsDontShow();

    bool checkPlaylistExists(std::string playlistName);
    bool shufflePlaylist();

    std::string getUsernameLinux();

    void displayPlayerControls();
};

#endif // FILE_MANAGER
