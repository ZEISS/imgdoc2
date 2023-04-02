// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    /// This interface is used for retrieving global information (about the document (i.e. usually aggregated from
    /// the individual tiles) specific to the 2d-document.
    public interface IDocInfo2d : IDocInfo
    {
        /// <summary>   
        /// Gets an axis aligned bounding box (of all tiles in the document). </summary>
        /// <returns>   The bounding box. </returns>
        Extent2d GetBoundingBox();
    }
}
