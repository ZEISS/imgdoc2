// SPDX-FileCopyrightText: 2024 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Implementation
{
    using System;
    using ImgDoc2Net.Interfaces;
    using ImgDoc2Net.Interop;

    /// <summary> Implementation of the IDecode interface.</summary>
    internal class Decoding : IDecoding
    {
        /// <inheritdoc/>
        public BitmapData Decode(Span<byte> compressedData, DataType dataType, PixelType pixelType, int width, int height, int stride = -1)
        {
            var (data, strideOfData) = ImgDoc2ApiInterop.Instance.Decode(compressedData, dataType, pixelType, width, height, stride);
            return new BitmapData()
            {
                Width = width,
                Height = height,
                Stride = strideOfData,
                PixelType = pixelType,
                Data = data,
            };
        }
    }
}
