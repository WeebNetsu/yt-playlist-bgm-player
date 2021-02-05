#if !defined(FILE_MANAGER)
#define FILE_MANAGER

#include <iostream>
#include <fstream>

class FileManager
{
private:
    std::string fileName;

public:
    FileManager(std::string fileName);
    ~FileManager();

    //getters
    std::string getFileName();
    std::string getTempFileName(std::string tempFileName);

    //aux
    bool checkFileEmpty();
    void createFile();
    std::ofstream openFile();
    void closeFile(std::ofstream fFile);

    void addPlaylist();
    void playPlaylist();
    void removePlaylist();
    void updatePlaylist();
    void instantPlayPlaylist(int playlistToPlay, bool shuffle);

    void updatePlaylistName(int playlistNumber);
    void updatePlaylistLink(int playlistNumber);

    std::vector<std::string> getAndShowPlaylists();
    std::vector<std::string> getPlaylistsDontShow();
    bool compareStrings(std::string str1, std::string str2);
    bool checkPlaylistExists(std::string playlistName);
    int inputSafe(int num);
    std::string getUsernameLinux();

    void displayPlayerControls();
};

#endif // FILE_MANAGER
