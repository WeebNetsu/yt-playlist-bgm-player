#ifndef COMMON_METHODS
#define COMMON_METHODS

#include <string>

namespace cmds
{
    int numberInput();

    bool compareStrings(std::string str1, std::string str2);

    void clearInput();
    void clearInput(int characters);
    void showMessage(std::string message, std::string type = "normal", bool breakLine = true);

    std::string trimStr(std::string str);
}

#endif // COMMON_METHODS
