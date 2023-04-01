// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#pragma pack(push, 4)
struct BrickBaseInfoInterop
{
    std::uint32_t pixelWidth;    ///< Width of the brick in unit of pixels.
    std::uint32_t pixelHeight;   ///< Height of the brick in unit of pixels.
    std::uint32_t pixelDepth;    ///< Depth of the brick in unit of pixels.

    /// The pixeltype of the brick. Commonly used values are defined in "pixeltype.h", but note that
    /// as far as the storage layer is concerned, this is an opaque value.
    std::uint8_t pixelType;
};
#pragma pack(pop)
