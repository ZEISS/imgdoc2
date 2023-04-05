// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include "brickbaseinfointerop.h"

#pragma pack(push, 4)
struct BrickBlobInfoInterop
{
    BrickBaseInfoInterop brick_base_info;

    std::uint8_t data_type; ///< The data type (corresponding to the enum "DataTypes" in DataTypes.h).
};
#pragma pack(pop)
