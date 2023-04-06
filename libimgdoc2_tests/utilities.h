// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT
#pragma once
<
#include <map>
#include <string>

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

/// A utility function to generate a unique filename (for SQLite) for a shared in memory database.
/// The name should be unique for each test case, so that the database is not shared between test cases.
/// The idea is to use the filename (per macro __FILE__) and the line number (per macro __LINE__) of 
/// the test case in order to generate a unique name.
/// This database is then constructed in memory and can be opened by another connection (c.f. https://www.sqlite.org/inmemorydb.html).
/// The memory is reclaimed when the last connection to the database closes.
///
/// \param  filename    The name of the source file (it is recommended to use the __FILE__ macro here).
/// \param  line_number The line number (recommended to use the __LINE__ macro).
///
/// \returns    The unique shared in memory file name for SQLite (something like "file:xyz23memdb?mode=memory&cache=shared").
std::string GenerateUniqueSharedInMemoryFileNameForSqlite(const char* filename, int line_number);
