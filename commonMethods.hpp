#ifndef COMMON_METHODS
#define COMMON_METHODS

#include <string>

namespace cmds
{
    int inputSafe(int num);
    bool compareStrings(std::string str1, std::string str2);
    void clearInput();
    void clearInput(int characters);
}

#endif // COMMON_METHODS
