#ifndef COMMON_METHODS
#define COMMON_METHODS

#include <string>

namespace cmds
{
    int numberInput();

    bool compareStrings(std::string str1, std::string str2);
    bool sii(std::string str); // sii = string is integer

    void clearInput();
    void clearInput(int characters);

    std::string skinStr(std::string str);

}

#endif // COMMON_METHODS
