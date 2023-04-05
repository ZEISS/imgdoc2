// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Implementation
{
    using System;
    using System.Collections.Generic;
    using System.Text;
    using ImgDoc2Net.Interfaces;
    using ImgDoc2Net.Interop;

    internal partial class Write3d : IWrite3d
    {
        private IntPtr writer3dObjectHandle;

        public Write3d(IntPtr handle)
        {
            this.writer3dObjectHandle = handle;
        }

        private Write3d()
        {
        }

        public long AddBrick(
            ITileCoordinate tileCoordinate,
            in LogicalPosition3d logicalPosition3d,
            Brick3dBaseInfo brick3dBaseInfo,
            DataType dataType,
            IntPtr pointerTileData,
            long sizeTileData)
        {
            return ImgDoc2ApiInterop.Instance.Writer3dAddBrick(
                this.writer3dObjectHandle,
                tileCoordinate,
                in logicalPosition3d,
                brick3dBaseInfo,
                dataType,
                pointerTileData,
                sizeTileData);
        }
    }

    /// <content>
    /// This part contains the implementation of IDisposable. 
    /// </content>
    internal partial class Write3d
    {
        public void Dispose()
        {
            this.Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            this.ReleaseUnmanagedResources();
            if (disposing)
            {
            }
        }

        private void ReleaseUnmanagedResources()
        {
            ImgDoc2ApiInterop.Instance.DestroyWriter3d(this.writer3dObjectHandle);
            this.writer3dObjectHandle = IntPtr.Zero;
        }
    }
}
