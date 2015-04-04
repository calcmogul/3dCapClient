// =============================================================================
// File Name: Util.hpp
// Description: Contains miscellaneous utility functions
// Author: Tyler Veness
// =============================================================================

#include <string>
#include <vector>

/* array of strings delimited by 'delim' is returned
 * 'num' determines how many strings to return. num = 0 doesn't impose limit on
 * number of strings
 */
const std::vector<std::string> split(const std::string& str, const
                                     std::string& delim, size_t num = 0);

