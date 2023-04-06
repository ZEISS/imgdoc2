// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT
#pragma once
#include <map>

// some pre-processor magic (-> https://gcc.gnu.org/onlinedocs/gcc-3.4.3/cpp/Stringification.html)
#define STRINGIFY(s) STRINGIFY_INTERNAL(s)
#define STRINGIFY_INTERNAL(s) #s

/// Simply utility for compare two maps for equality.
///
/// \tparam Tkey   Type of the map's key.
/// \tparam Tvalue Type of the map's value.
/// \param  map1 The first map.
/// \param  map2 The second map.
///
/// \returns True if it the maps are equal, false if not.
template <typename Tkey, typename Tvalue>
bool mapsEqual(const std::map<Tkey, Tvalue>& map1, const std::map<Tkey, Tvalue>& map2)
{
    if (map1.size() != map2.size()) 
    {
        return false;
    }

    for (auto it = map1.begin(); it != map1.end(); ++it) 
    {
        auto it2 = map2.find(it->first);
        if (it2 == map2.end() || it2->second != it->second) 
        {
            return false;
        }
    }

    return true;
}
