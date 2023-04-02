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

        /// <summary> 
        /// Gets minimum and maximum value for the specified tile dimensions.
        /// If the minimum/maximum cannot be determined (for a dimension), then the result will be
        /// one where Minimum is greater than Maximum. This can happen e.g. if the document is empty, or
        /// the coordinates are Null.
        /// </summary>
        /// <param name="dimensions"> The dimensions to query the min/max for.</param>
        /// <returns> 
        /// A dictionary with the key 'dimension' and the value a tuple containing the minimum and the maximum. A minimum
        /// greater than the maximum means that the value is indeterminate.
        /// </returns>
        Dictionary<Dimension, (int Minimum, int Maximum)> GetMinMaxForTileDimension(IEnumerable<Dimension> dimensions);

        /// <summary> Gets total number of tiles in the document.</summary>
        /// <returns> The total number of tiles.</returns>
        long GetTotalNumberOfTiles();

        /// <summary> Gets the total number of tiles per pyramid layer.</summary>
        /// <returns> A dictionary, where key is the pyramid layer number, and value is the total number of tiles (on this layer) in the document. </returns>
        Dictionary<int, long> GetTileCountPerPyramidLayer();
    }
}
