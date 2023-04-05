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

    /// <summary> Implementation of the ICreateOptions interface.</summary>
    public partial class CreateOptions : ICreateOptions
    {
        private IntPtr objectHandle;

        /// <summary>
        /// Initializes a new instance of the <see cref="CreateOptions"/> class.
        /// </summary>
        public CreateOptions()
        {
            this.objectHandle = ImgDoc2ApiInterop.Instance.CreateCreateOptions();
        }

        /// <summary> Gets the interop handle.</summary>
        /// <remarks> This is intended for internal use. </remarks>
        /// <value> The handle.</value>
        public IntPtr Handle
        {
            get { return this.objectHandle; }
        }

        public DocumentType DocumentType
        {
            get { return ImgDoc2ApiInterop.Instance.CreateOptionsGetDocumentType(this.objectHandle); }
            set { ImgDoc2ApiInterop.Instance.CreateOptionsSetDocumentType(this.objectHandle, value); }
        }

        /// <summary> Gets or sets the filename of the file.</summary>
        ///
        /// <value> The filename.</value>
        public string Filename
        {
            get { return ImgDoc2ApiInterop.Instance.CreateOptionsGetFilename(this.objectHandle); }
            set { ImgDoc2ApiInterop.Instance.CreateOptionsSetFilename(this.objectHandle, value); }
        }

        /// <summary> Gets or sets a value indicating whether the database should be created containing a
        ///     spatial index.</summary>
        ///
        /// <value> True if to construct with a spatial index; false otherwise.</value>
        public bool UseSpatialIndex
        {
            get { return ImgDoc2ApiInterop.Instance.CreateOptionsGetUseSpatialIndex(this.objectHandle); }
            set { ImgDoc2ApiInterop.Instance.CreateOptionsSetUseSpatialIndex(this.objectHandle, value); }
        }

        /// <summary> Gets or sets a value indicating whether the database should be created containing a blob table.</summary>
        ///
        /// <value> True if a blob table is to be created; false if not.</value>
        public bool UseBlobTable
        {
            get { return ImgDoc2ApiInterop.Instance.CreateOptionsGetUseBlobTable(this.objectHandle); }
            set { ImgDoc2ApiInterop.Instance.CreateOptionsSetUseBlobTable(this.objectHandle, value); }
        }

        /// <inheritdoc/>
        public void AddDimension(Dimension dimension)
        {
            ImgDoc2ApiInterop.Instance.CreateOptionsAddDimension(this.objectHandle, dimension);
        }

        /// <summary> Adds the enumeration of dimensions.</summary>
        ///
        /// <param name="dimensions"> The enumeration of dimensions.</param>
        public void AddDimensions(IEnumerable<Dimension> dimensions)
        {
            foreach (var d in dimensions)
            {
                this.AddDimension(d);
            }
        }

        /// <inheritdoc/>
        public void AddIndexedDimension(Dimension dimension)
        {
            ImgDoc2ApiInterop.Instance.CreateOptionsAddIndexedDimension(this.objectHandle, dimension);
        }

        /// <summary> Adds the enumeration of dimensions for which an index is to be created.</summary>
        ///
        /// <param name="dimensions"> The enumeration of dimensions for which an index is to be created.</param>
        public void AddIndexedDimensions(IEnumerable<Dimension> dimensions)
        {
            foreach (var d in dimensions)
            {
                this.AddIndexedDimension(d);
            }
        }
    }

    /// <content>
    /// This part contains the implementation of IDisposable. 
    /// </content>
    public partial class CreateOptions
    {
        /// <summary>
        /// Finalizes an instance of the <see cref="CreateOptions"/> class.
        /// </summary>
        ~CreateOptions()
        {
            this.Dispose(false);
        }
        
        /// <summary> Performs application-defined tasks associated with freeing, releasing, or resetting
        ///     unmanaged resources.</summary>
        public void Dispose()
        {
            this.Dispose(true);
            GC.SuppressFinalize(this);
        }

        /// <summary> Performs application-defined tasks associated with freeing, releasing, or resetting
        ///     unmanaged resources.</summary>
        ///
        /// <param name="disposing"> True to release both managed and unmanaged resources; false to
        ///     release only unmanaged resources.</param>
        protected virtual void Dispose(bool disposing)
        {
            this.ReleaseUnmanagedResources();
            if (disposing)
            {
            }
        }

        /// <summary> Releases the unmanaged resources.</summary>
        private void ReleaseUnmanagedResources()
        {
            ImgDoc2ApiInterop.Instance.DestroyCreateOptions(this.objectHandle);
            this.objectHandle = IntPtr.Zero;
        }
    }
}
