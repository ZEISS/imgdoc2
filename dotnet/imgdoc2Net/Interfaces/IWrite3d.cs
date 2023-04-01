// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    using System;

    /// <summary>   
    /// This interface is used to give write-access to the document-3D. 
    /// </summary>
    public interface IWrite3d : IDisposable
    {
        /// <summary>   
        /// Adds a brick with the specified content. 
        /// </summary>
        /// <param name="tileCoordinate">       The brick coordinate. </param>
        /// <param name="logicalPosition3d">    The logical position. </param>
        /// <param name="brick3dBaseInfo">      Information describing the bricks's bitmap. </param>
        /// <param name="dataType">             The data type (or type if representation) of the pixel data. </param>
        /// <param name="pointerTileData">      Pointer to the pixel data. </param>
        /// <param name="sizeTileData">         The size of the data (pointed to by "pointerTileData". </param>
        /// <returns type="long">   The primary key of the newly added tile. </returns>
        long AddBrick(
            ITileCoordinate tileCoordinate,
            in LogicalPosition3d logicalPosition3d,
            Brick3dBaseInfo brick3dBaseInfo,
            DataType dataType,
            IntPtr pointerTileData,
            long sizeTileData);
    }
}
