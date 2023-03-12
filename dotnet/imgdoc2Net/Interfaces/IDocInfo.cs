// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    using System.Collections.Generic;
    using ImgDoc2Net.Implementation;

    /// <summary> 
    /// This interface is used for retrieving information about the document.
    /// </summary>
    public interface IDocInfo
    {
        /// <summary> 
        /// Get the tile-dimensions of the document. This is corresponding to the native method 'IDocInfo::GetTileDimensions'.
        /// </summary>
        /// <returns> An array with the dimensions used in the document. </returns>
        Dimension[] GetTileDimensions();

        Dictionary<Dimension, (int Minimum, int Maximum)> GetMinMaxForTileDimension(IEnumerable<Dimension> dimensions);
    }
}
