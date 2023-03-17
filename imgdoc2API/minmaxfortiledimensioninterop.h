// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>

#pragma pack(push, 4)
/// This struct is used for specifying an interval. If minimum_value is greater than maximum_value,
/// it means that the interval is invalid. 
struct MinMaxForTilePositionsInterop
{
    std::int32_t minimum_value;  ///< The minimum value.
    std::int32_t maximum_value;  ///< The maximum value.
};
#pragma pack(pop)
