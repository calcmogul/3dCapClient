// =============================================================================
// File Name: Util.cpp
// Description: Contains miscellaneous utility functions
// Author: Tyler Veness
// =============================================================================

#include "Util.hpp"

const std::vector<std::string> split(const std::string& str, const
                                     std::string& delim, size_t num) {
    std::vector<std::string> arr;
    size_t count = 0;

    for (size_t pos = 0; (num == 0 || count < num) && pos < str.length();
         pos++) {
        arr.push_back(str.substr(pos, str.find(delim, pos)));
        count++;

        while (pos < str.length() && str.substr(pos, delim.length()) !=
               delim) {
            pos += delim.length();
        }
    }

    return arr;
}

