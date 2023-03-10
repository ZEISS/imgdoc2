// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>

#pragma pack(push, 4)
struct TileBaseInfoInterop
{
    std::uint32_t pixelWidth;
    std::uint32_t pixelHeight;
    std::uint8_t pixelType;
};
#pragma pack(pop)
