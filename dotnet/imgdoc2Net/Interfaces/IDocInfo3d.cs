// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    /// This interface is used for retrieving global information (about the document (i.e. usually aggregated from
    /// the individual bricks) specific to the 3d-document.
    public interface IDocInfo3d : IDocInfo
    {
        /// <summary>   
        /// Gets an axis aligned bounding cuboid (of all bricks in the document). </summary>
        /// <returns>   The bounding box. </returns>
        Extent3d GetBoundingBox();
    }
}
