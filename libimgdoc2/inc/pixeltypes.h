// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>

namespace imgdoc2
{
    /// \brief definition of pixel types
    ///        
    /// Those constants are used in the TileBaseInfo structure. This is the definition
    /// of the pixel types.
    class PixelType
    {
    public:
        static constexpr std::uint8_t Unknown = 0;      ///< This constant is reserved for representing an unknown pixel type.
        static constexpr std::uint8_t Gray8 = 1;        ///< This constant is representing the pixel type "gray8". A pixel is represented by an unsigned byte.
        static constexpr std::uint8_t Gray16 = 2;       ///< This constant is representing the pixel type "gray16". A pixel is represented by an unsigned word.
        static constexpr std::uint8_t Bgr24 = 3;        ///< This constant is representing the pixel type "BGR24". A pixel is represented by a triple of 3 unsigned bytes.
        static constexpr std::uint8_t Bgr48 = 4;        ///< This constant is representing the pixel type "BGR48". A pixel is represented by a triple of 3 unsigned words.
        static constexpr std::uint8_t Gray32Float = 5;  ///< This constant is representing the pixel type "gray32float". A pixel is represented by a 32-bit float.
    };
}
