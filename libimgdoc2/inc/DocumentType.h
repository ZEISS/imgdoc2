// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

namespace imgdoc2
{
    /// Values that represent different document types.
    enum class DocumentType
    {
        kInvalid = 0,    ///< An enum constant representing the invalid option.
        kImage2d,        ///< An enum constant representing the "image 2D" option.
        kImage3d         ///< An enum constant representing the "image 3D" option. NOT YET FULLY IMPLEMENTED.
    };
}
