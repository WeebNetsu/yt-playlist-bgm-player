#if !defined(MEDIA_PLAYER)
#define MEDIA_PLAYER

#include <iostream>
#include <fstream>
#include <map>

class Player
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
    Player(std::string fileName = "custom-playlists.txt");
    ~Player();

    std::string getFileName();
    std::string getFileName(std::string tempFileName);

    void setup();
    void addPlaylist();
    void playPlaylists();
    void randomPlayPlaylists(std::map<std::string, bool> flags);
    void showPlaylists();
    void removePlaylist();
    void updatePlaylist();
    void instantPlayPlaylists(std::vector<int> playlistsToPlay, std::map<std::string, bool> flags);

    void updatePlaylistName(int playlistNumber);
    void updatePlaylistLink(int playlistNumber);

    std::string rewritePlaylists(int playlistNumber, ModType modType, std::string newChange);
    void rewriteFile(std::string newList);

    std::vector<std::string> getAndShowPlaylists();
    std::vector<std::string> getPlaylistsDontShow();
    std::vector<std::string> generateCleanList(std::vector<int> playlistsToPlay, bool usingMenu);

    bool checkPlaylistExists(std::string playlistName);
    bool shufflePlaylist();
    bool checkFileEmpty();

    std::string getUsernameLinux();

    void displayPlayerControls();
};

#endif // MEDIA_PLAYER
