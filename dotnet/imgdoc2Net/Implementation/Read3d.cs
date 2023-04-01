// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Implementation
{
    using System;
    using System.Collections.Generic;
    using ImgDoc2Net.Interfaces;
    using ImgDoc2Net.Interop;

    internal partial class Read3d : IRead3d
    {
        private IntPtr reader3dObjectHandle;

        /// <summary>   Initializes a new instance of the <see cref="Read2d"/> class. </summary>
        /// <param name="handle" type="IntPtr"> The handle. </param>
        public Read3d(IntPtr handle)
        {
            this.reader3dObjectHandle = handle;
        }

        private Read3d()
        {
        }

        public Extent2d GetBoundingBox()
        {
            throw new NotImplementedException();
        }

        public Dictionary<Dimension, (int Minimum, int Maximum)> GetMinMaxForTileDimension(IEnumerable<Dimension> dimensions)
        {
            throw new NotImplementedException();
        }

        public Dictionary<int, long> GetTileCountPerPyramidLayer()
        {
            throw new NotImplementedException();
        }

        public Dimension[] GetTileDimensions()
        {
            throw new NotImplementedException();
        }

        public long GetTotalNumberOfTiles()
        {
            throw new NotImplementedException();
        }
    }

    /// <content>
    /// This part contains the implementation of IDisposable. 
    /// </content>
    internal partial class Read3d
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
            ImgDoc2ApiInterop.Instance.DestroyReader2d(this.reader3dObjectHandle);
            this.reader3dObjectHandle = IntPtr.Zero;
        }
    }
}
