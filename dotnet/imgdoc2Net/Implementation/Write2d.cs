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

    /// <summary>   
    /// Implementation of the "IWrite2d" interface. 
    /// </summary>
    /// <remarks>
    /// TODO: guard against multiple dispose-calls
    /// </remarks>
    internal partial class Write2d : IWrite2d
    {
        private IntPtr writer2dObjectHandle;

        /// <summary> 
        /// Initializes a new instance of the <see cref="Write2d"/> class.
        /// </summary>
        /// <param name="handle"> The handle of a writer2d object.</param>
        public Write2d(IntPtr handle)
        {
            this.writer2dObjectHandle = handle;
        }

        /// <summary> Prevents a default instance of the <see cref="Write2d"/> class from being created.</summary>
        private Write2d()
        {
        }

        /// <inheritdoc/>
        public long AddTile(
            ITileCoordinate tileCoordinate,
            in LogicalPosition logicalPosition,
            Tile2dBaseInfo tile2dBaseInfo,
            DataType dataType,
            IntPtr pointerTileData,
            long sizeTileData)
        {
            return ImgDoc2ApiInterop.Instance.Writer2dAddTile(
                this.writer2dObjectHandle,
                tileCoordinate,
                in logicalPosition,
                tile2dBaseInfo,
                dataType,
                pointerTileData,
                sizeTileData);
        }
    }

    /// <content>
    /// This part contains the implementation of ITransaction. 
    /// </content>
    internal partial class Write2d
    {
        /// <inheritdoc/>
        public void BeginTransaction()
        {
            ImgDoc2ApiInterop.Instance.Writer2dBeginTransaction(this.writer2dObjectHandle);
        }

        /// <inheritdoc/>
        public void CommitTransaction()
        {
            ImgDoc2ApiInterop.Instance.Writer2dCommitTransaction(this.writer2dObjectHandle);
        }

        /// <inheritdoc/>
        public void RollbackTransaction()
        {
            ImgDoc2ApiInterop.Instance.Writer2dRollbackTransaction(this.writer2dObjectHandle);
        }
    }

    /// <content>
    /// This part contains the implementation of IDisposable. 
    /// </content>
    internal partial class Write2d
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
            ImgDoc2ApiInterop.Instance.DestroyWriter2d(this.writer2dObjectHandle);
            this.writer2dObjectHandle = IntPtr.Zero;
        }
    }
}
