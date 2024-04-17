// SPDX-FileCopyrightText: 2024 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    using System;

    /// <summary> 
    /// Interface for decoding compressed data.
    /// </summary>
    public interface IDecoding
    {
        /// <summary> Decode compressed bitmap-data into an uncompressed bitmap.</summary>
        /// <param name="compressedData"> The compressed data.</param>
        /// <param name="dataType">       Type of the data.</param>
        /// <param name="pixelType">      The pixel type (of the uncompressed bitmap).</param>
        /// <param name="width">          The width in pixels.</param>
        /// <param name="height">         The height in pixels.</param>
        /// <param name="stride">         (Optional) The stride of the output bitmap. If this is negative, then an arbitrary stride must be assumed (for the returned bitmap).</param>
        /// <returns> A BitmapData structure.</returns>
        BitmapData Decode(Span<byte> compressedData, DataType dataType, PixelType pixelType, int width, int height, int stride = -1);
    }
}
