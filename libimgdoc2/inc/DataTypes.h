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

        /// The binary blob is a JPG-XR compressed bitmap.
        JPGXRCOMPRESSED_BITMAP = 2,

        /// The binary blob is a bitmap compressed with zstd.
        ZSTD0COMPRESSED_BITMAP = 3,

        /// The binary blob is a bitmap compressed with "zstd1"-compression. This is a variant of the zstd-compression which includes a header
        /// and (optionally) some pre-processing of the data.
        ZSTD1COMPRESSED_BITMAP = 4,

        UNCOMPRESSED_BRICK = 32,
        CUSTOM = 255
    };

}
