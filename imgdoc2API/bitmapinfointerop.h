/// SPDX-FileCopyrightText: 2024 Carl Zeiss Microscopy GmbH
/// 
/// SPDX-License-Identifier: MIT
#pragma once

#pragma pack(push, 4)
struct BitmapInfoInterop
{
    std::uint8_t pixelType;
    std::uint32_t pixelWidth;
    std::uint32_t pixelHeight;
};
#pragma pack(pop)
