// Copyright (c) Tyler Veness

#pragma once

#include <string_view>
#include <vector>

/// Split string on delimiter.
///
/// @param str String to split.
/// @param delim Delimiter to split on.
/// @param num How many splits to return. 0 imposes no limit.
inline std::vector<std::string_view> split(std::string_view str,
                                           std::string_view delim,
                                           size_t num = 0) {
    std::vector<std::string_view> arr;
    size_t count = 0;

    for (size_t pos = 0; (num == 0 || count < num) && pos < str.size(); ++pos) {
        arr.push_back(str.substr(pos, str.find(delim, pos)));
        ++count;

        while (pos < str.size() && str.substr(pos, delim.size()) != delim) {
            pos += delim.size();
        }
    }

    return arr;
}
