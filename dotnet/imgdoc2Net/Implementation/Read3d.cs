// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Implementation
{
    using System;
    using System.Collections.Generic;
    using System.Drawing;
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

        /// <inheritdoc/>
        public List<long> Query(IDimensionQueryClause queryClause, ITileInfoQueryClause tileInfoQueryClause, QueryOptions queryOptions)
        {
            // TODO(Jbl): error-handling
            var queryResult = ImgDoc2ApiInterop.Instance.Reader3dQuery(
                this.reader3dObjectHandle,
                queryClause,
                tileInfoQueryClause,
                queryOptions != null ? queryOptions.MaxNumbersOfResults : QueryOptions.DefaultMaxNumberOfResults);
            if (queryOptions != null)
            {
                queryOptions.ResultWasComplete = queryResult.ResultComplete;
            }

            return queryResult.Keys;
        }

        /// <inheritdoc/>
        public List<long> QueryTilesIntersectingRect(Cuboid cuboid, IDimensionQueryClause queryClause, ITileInfoQueryClause tileInfoQueryClause, QueryOptions queryOptions)
        {
            // TODO(Jbl): error-handling
            var queryResult = ImgDoc2ApiInterop.Instance.Reader3dQueryBricksIntersectingCuboid(
                this.reader3dObjectHandle,
                cuboid,
                queryClause,
                tileInfoQueryClause,
                queryOptions != null ? queryOptions.MaxNumbersOfResults : QueryOptions.DefaultMaxNumberOfResults);
            if (queryOptions != null)
            {
                queryOptions.ResultWasComplete = queryResult.ResultComplete;
            }

            return queryResult.Keys;
        }

        /// <inheritdoc/>
        public List<long> QueryTilesIntersectingPlane(PlaneHesse plane, IDimensionQueryClause queryClause, ITileInfoQueryClause tileInfoQueryClause, QueryOptions queryOptions)
        {
            // TODO(Jbl): error-handling
            var queryResult = ImgDoc2ApiInterop.Instance.Reader3dQueryBricksIntersectingPlane(
                this.reader3dObjectHandle,
                plane,
                queryClause,
                tileInfoQueryClause,
                queryOptions != null ? queryOptions.MaxNumbersOfResults : QueryOptions.DefaultMaxNumberOfResults);
            if (queryOptions != null)
            {
                queryOptions.ResultWasComplete = queryResult.ResultComplete;
            }

            return queryResult.Keys;
        }

        /// <inheritdoc/>
        public byte[] ReadBrickData(long key)
        {
            // TODO(Jbl): error-handling
            return ImgDoc2ApiInterop.Instance.Reader3dReadBrickData(this.reader3dObjectHandle, key);
        }

        public Dictionary<Dimension, (int Minimum, int Maximum)> GetMinMaxForTileDimension(IEnumerable<Dimension> dimensions)
        {
            return ImgDoc2ApiInterop.Instance.DocInfoGetMinMaxForTileDimensions(this.reader3dObjectHandle, dimensions);
        }

        public Dictionary<int, long> GetTileCountPerPyramidLayer()
        {
            throw new NotImplementedException();
        }

        public Dimension[] GetTileDimensions()
        {
            return ImgDoc2ApiInterop.Instance.DocInfo3dGetTileDimensions(this.reader3dObjectHandle);
        }

        public long GetTotalNumberOfTiles()
        {
            throw new NotImplementedException();
        }

        public Extent3d GetBoundingBox()
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
