// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Implementation
{
    using System;
    using ImgDoc2Net.Interfaces;
    using ImgDoc2Net.Interop;

    /// <summary> Implementation of the IOpenExistingOptions interface.</summary>
    public partial class OpenExistingOptions : IOpenExistingOptions
    {
        private IntPtr objectHandle;

        /// <summary>
        /// Initializes a new instance of the <see cref="OpenExistingOptions"/> class. </summary>
        public OpenExistingOptions()
        {
            this.objectHandle = ImgDoc2ApiInterop.Instance.CreateOpenExistingOptions();
        }

        /// <summary> Gets the interop handle.</summary>
        /// <remarks> This is intended for internal use. </remarks>
        /// <value> The handle.</value>
        public IntPtr Handle
        {
            get { return this.objectHandle; }
        }

        /// <summary> Gets or sets the filename of the file.</summary>
        ///
        /// <value> The filename.</value>
        public string Filename
        {
            get { return ImgDoc2ApiInterop.Instance.OpenExistingOptionsGetFilename(this.objectHandle); }
            set { ImgDoc2ApiInterop.Instance.OpenExistingOptionsSetFilename(this.objectHandle, value); }
        }

        /// <summary> Gets a value indicating whether the file is to opened as "readonly".</summary>
        ///
        /// <value> True file is to opened as "readonly"; false otherwise.</value>
        public bool Readonly => throw new NotImplementedException();
    }

    /// <content>
    /// This part contains the implementation of IDisposable. 
    /// </content>
    public partial class OpenExistingOptions
    {
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
        }

        private void ReleaseUnmanagedResources()
        {
            ImgDoc2ApiInterop.Instance.DestroyOpenExistingOptions(this.objectHandle);
            this.objectHandle = IntPtr.Zero;
        }
    }
}
