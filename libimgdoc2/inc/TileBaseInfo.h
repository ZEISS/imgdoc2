// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include "pixeltypes.h"
#include "DataTypes.h"

namespace imgdoc2
{
    /// This constitutes the "base information" of a tile. A tile in this respect is a rectangular
    /// bitmap, characterized by it width and height and pixels and a pixeltype.
    /// Pixeltype is an opaque property as far as the "storage layer" is concerned.
    struct TileBaseInfo
    {
        std::uint32_t pixelWidth{0};    ///< Width of the tile in unit of pixels.
        std::uint32_t pixelHeight{0};   ///< Height of the tile in unit of pixels.
        
        /// The pixeltype of the tile. Commonly used values are defined in "pixeltype.h", but note that
        /// as far as the storage layer is concerned, this is an opaque value.
        std::uint8_t pixelType{ imgdoc2::PixelType::Unknown};
    };

    /// This gives the tile blob information - the base information we can provide about the bitmap
    /// contained in the corresponding blob. This information is part of the database and is available
    /// without inspecting/decoding the blob itself.
    struct TileBlobInfo
    {
        TileBaseInfo base_info;              ///< The tile base info, i.e. width, height and pixel type.
        imgdoc2::DataTypes data_type {0};    ///< Information about the data type of the blob, i.e. in which binary representation the bitmap is stored.
    };
}
