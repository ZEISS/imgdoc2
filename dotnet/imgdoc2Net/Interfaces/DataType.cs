// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    /// <summary> 
    /// Values that represent the "imgdoc2-data types" of tiles.
    /// Keep this in sync with the respective native enum, and the conversion-method in imgdoc2API/utilities.cpp.
    /// </summary>
    public enum DataType : byte
    {
        /// <summary>   
        /// The zero data-type. This tile is to be interpreted as "all pixels are zero", irrespective of the
        /// content of the data-blob (it may not even be present).
        /// </summary>
        Zero = 0,

        /// <summary>    
        /// The binary blob is an uncompressed bitmap. 
        /// </summary>
        UncompressedBitmap = 1,

        JpgXrCompressedBitmap = 2,

        UncompressedBrick = 32,
    }
}
