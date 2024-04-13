// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Implementation
{
    using System;
    using ImgDoc2Net.Implementation;
    using ImgDoc2Net.Interfaces;
    using ImgDoc2Net.Interop;

    /// <summary> 
    /// The imgdoc2 document object.
    /// </summary>
    public partial class Document : IDocument
    {
        private readonly IntPtr documentHandle;

        /// <summary>
        /// Initializes a new instance of the <see cref="Document"/> class. 
        /// </summary>
        private Document()
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Document"/> class.
        /// </summary>
        /// <param name="createOptions"> Options for controlling the create operation.</param>
        private Document(CreateOptions createOptions)
        {
            this.documentHandle = ImgDoc2ApiInterop.Instance.CreateNewDocument(createOptions.Handle);
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="Document"/> class.
        /// </summary>
        /// <param name="openExistingOptions"> Options for controlling the open-existing operation.</param>
        private Document(OpenExistingOptions openExistingOptions)
        {
            this.documentHandle = ImgDoc2ApiInterop.Instance.OpenExistingDocument(openExistingOptions.Handle);
        }

        /// <summary> Gets reader-object in case of a 2d-image document.</summary>
        ///
        /// <returns> The reader-object for a 2d-image-document.</returns>
        public IRead2d Get2dReader()
        {
            var readerHandle = ImgDoc2ApiInterop.Instance.DocumentGetReader2d(this.documentHandle);
            if (readerHandle != IntPtr.Zero)
            {
                return new Read2d(readerHandle);
            }

            return null;
        }

        /// <summary> Gets writer-object in case of a 2d-image document.</summary>
        ///
        /// <returns> The writer-object for a 2d-image-document.</returns>
        public IWrite2d Get2dWriter()
        {
            var writerHandle = ImgDoc2ApiInterop.Instance.DocumentGetWriter2d(this.documentHandle);
            if (writerHandle != IntPtr.Zero)
            {
                return new Write2d(writerHandle);
            }

            return null;
        }

        /// <inheritdoc/>
        public IRead3d Get3dReader()
        {
            var readerHandle = ImgDoc2ApiInterop.Instance.DocumentGetReader3d(this.documentHandle);
            if (readerHandle != IntPtr.Zero)
            {
                return new Read3d(readerHandle);
            }

            return null;
        }

        /// <inheritdoc/>
        public IWrite3d Get3dWriter()
        {
            var writerHandle = ImgDoc2ApiInterop.Instance.DocumentGetWriter3d(this.documentHandle);
            if (writerHandle != IntPtr.Zero)
            {
                return new Write3d(writerHandle);
            }

            return null;
        }
    }

    /// <content>   
    /// This part contains the implementation of the factory methods. 
    /// </content>
    public partial class Document
    {
        /// <summary> Creates a new document.</summary>
        ///
        /// <param name="createOptions"> Options for controlling the creation of the new document.</param>
        ///
        /// <returns> The newly created document.</returns>
        public static Document CreateNew(CreateOptions createOptions)
        {
            return new Document(createOptions);
        }

        /// <summary> Opens an existing document.</summary>
        ///
        /// <param name="openExistingOptions"> Options for controlling the open-existing operation.</param>
        ///
        /// <returns> The newly created document.</returns>
        public static Document OpenExisting(OpenExistingOptions openExistingOptions)
        {
            return new Document(openExistingOptions);
        }
    }

    /// <content>   This part contains the implementation of the IDisposable interface. </content>
    public partial class Document
    {
        private bool disposedValue;

        /// <summary>   
        /// Finalizes an instance of the <see cref="Document"/> class..
        /// </summary>
        ~Document()
        {
            // Do not change this code. Put cleanup code in 'Dispose(bool disposing)' method
            this.Dispose(disposing: false);
        }

        /// <summary>
        /// Performs application-defined tasks associated with freeing, releasing, or resetting unmanaged
        /// resources.
        /// </summary>
        public void Dispose()
        {
            // Do not change this code. Put cleanup code in 'Dispose(bool disposing)' method
            this.Dispose(disposing: true);
            GC.SuppressFinalize(this);
        }

        /// <summary> Performs application-defined tasks associated with freeing, releasing, or resetting
        ///     unmanaged resources.</summary>
        ///
        /// <param name="disposing"> True to release both managed and unmanaged resources; false to
        ///     release only unmanaged resources.</param>
        protected virtual void Dispose(bool disposing)
        {
            if (!this.disposedValue)
            {
                if (disposing)
                {
                    // TODO: dispose managed state (managed objects)
                }

                ImgDoc2ApiInterop.Instance.DestroyDocument(this.documentHandle);
                
                // TODO: set large fields to null
                this.disposedValue = true;
            }
        }
    }
}
