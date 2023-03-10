// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    /// <summary>   
    /// This enumeration lists the pixel types. 
    /// This is corresponding to the file "pixeltypes.h".
    /// </summary>
    public enum PixelType 
    {
        /// <summary> 
        /// An enum constant representing an invalid pixel type.
        /// </summary>
        None = 0,

        /// <summary> 
        /// An enum constant representing the pixel type "gray8". A pixel is represented by an unsigned byte.
        /// </summary>
        Gray8 = 1,

        /// <summary> 
        /// An enum constant representing the pixel type "gray16". A pixel is represented by an unsigned word.
        /// </summary>
        Gray16 = 2,

        /// <summary> 
        /// An enum constant representing the pixel type "BGR24". A pixel is represented by a triple of 3 unsigned bytes.
        /// </summary>
        Bgr24 = 3,

        /// <summary> 
        /// An enum constant representing the pixel type "BGR48". A pixel is represented by a triple of 3 unsigned words.
        /// </summary>
        Bgr48 = 4,

        /// <summary> 
        /// An enum constant representing the pixel type "gray32float". A pixel is represented by a 32-bit float.
        /// </summary>
        Gray32Float = 5,
    }
}
