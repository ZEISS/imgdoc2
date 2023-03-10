// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>

#pragma pack(push, 4)
struct PyramidLevelConditionInterop
{
    std::uint8_t logical_operator;
    std::uint8_t comparison_operator;
    std::int32_t value;
};

struct TileInfoQueryClauseInterop
{
    int pyramid_level_condition_count;
    PyramidLevelConditionInterop pyramid_level_condition[];
};
#pragma pack(pop)
