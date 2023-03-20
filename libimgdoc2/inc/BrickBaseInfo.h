// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include "pixeltypes.h"
#include "DataTypes.h"

namespace imgdoc2
{
    /// This constitutes the "base information" of a brick. A brick in this respect is a axis-aligned cuboid
    /// 3D-bitmap, characterized by its width, height, depth and voxels and a pixeltype.
    /// Pixeltype is an opaque property as far as the "storage layer" is concerned.
    struct BrickBaseInfo
    {
        std::uint32_t pixelWidth{ 0 };    ///< Width of the brick in unit of pixels.
        std::uint32_t pixelHeight{ 0 };   ///< Height of the brick in unit of pixels.
        std::uint32_t pixelDepth{ 0 };    ///< Depth of the brick in unit of pixels.

        /// The pixeltype of the brick. Commonly used values are defined in "pixeltype.h", but note that
        /// as far as the storage layer is concerned, this is an opaque value.
        std::uint8_t pixelType{ imgdoc2::PixelType::Unknown };
    };

    /// This gives the brick blob information - the base information we can provide about the bitmap
    /// contained in the corresponding blob. This information is part of the database and is available
    /// without inspecting/decoding the blob itself.
    struct BrickBlobInfo
    {
        BrickBaseInfo base_info;              ///< The brick base info, i.e. width, height, depth and pixel type.
        imgdoc2::DataTypes data_type{ 0 };    ///< Information about the data type of the blob, i.e. in which binary representation the bitmap is stored.
    };
}
