// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

namespace imgdoc2
{
    /// This enumeration specifies the "type" of the tile-binary-data. I.e. how the binary blob associated with a tile is to be dealt with.
    enum class DataTypes : std::uint8_t
    {
        /// The zero data-type. This tile is to be interpreted as "all pixels are zero", irrespective of the
        /// content of the data-blob (it may not even be present).
        ZERO = 0,

        /// The binary blob is an uncompressed bitmap.
        /// TODO: describe details, like requirements wrt to stride, orientation etc.
        UNCOMPRESSED_BITMAP = 1,

        JPGXRCOMPRESSED_BITMAP = 2,

        UNCOMPRESSED_BRICK = 32,
        CUSTOM = 255
    };

}
