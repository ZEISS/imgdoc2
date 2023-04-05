// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interfaces
{
    using System;
    using System.Collections.Generic;
    using System.Text;

    /// <summary>   
    /// In this class we gather extensions methods (on the IWrite3d-interface). 
    /// </summary>
    public static class Write3dExtensions
    {
        public static long AddBrick(
            this IWrite3d write3d,
            ITileCoordinate tileCoordinate,
            in LogicalPosition3d logicalPosition3d,
            Brick3dBaseInfo brick3dBaseInfo,
            DataType dataType,
            byte[] data)
        {
            if (data != null)
            {
                unsafe
                {
                    fixed (byte* pointerToData = &data[0])
                    {
                        return write3d.AddBrick(tileCoordinate, in logicalPosition3d, brick3dBaseInfo, dataType, new IntPtr(pointerToData), data.Length);
                    }
                }
            }
            else
            {
                return write3d.AddBrick(tileCoordinate, in logicalPosition3d, brick3dBaseInfo, dataType, IntPtr.Zero, 0);
            }
        }
    }
}
