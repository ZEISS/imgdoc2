// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interop
{
    using System;

    /// <summary>   
    /// This abstract class is used to handle loading a native DLL.
    /// </summary>
    internal abstract class DllLoader 
    {
        private string filename;
        private IntPtr dllHandle = IntPtr.Zero;

        /// <summary>   
        /// Initializes a new instance of the <see cref="DllLoader"/> class.
        /// Specialized constructor for use only by derived class. Here we store the filename of the dynamic library to be loaded. 
        /// </summary>
        /// <param name="filename" type="string">   Filename of the file. </param>
        protected DllLoader(string filename)
        {
            this.filename = filename;
        }

        /// <summary>   Gets or sets the OS-handle of the DLL/dynlib. </summary>
        /// <value> The OS-handle representing the DLL/dynlib. </value>
        protected IntPtr DllHandle
        {
            get { return this.dllHandle; }
            set { this.dllHandle = value; }
        }

        /// <summary>   Gets or sets the filename of the DLL/dynlib. </summary>
        /// <value> The filename of the DLL/dynlib. </value>
        protected string Filename
        {
            get { return this.filename; }
            set { this.filename = value; }
        }

        /// <summary>   
        /// Gets an DLL-loader-instance (appropriate for the platform). 
        /// </summary>
        /// <param name="filename" type="string">   Filename of the file. </param>
        /// <returns type="DllLoader">  The newly created DLL-loader instance. </returns>
        public static DllLoader GetDllLoader(string filename)
        {
            if (Utilities.IsLinux())
            {
                return new DllLoaderLinux(filename);
            }

            return new DllLoaderWindows(filename);
        }

        /// <summary>   Attempts to load the DLL/dynlib. </summary>
        /// <exception cref="InvalidOperationException">    Thrown when the requested operation is
        ///                                                 invalid. </exception>
        /// <exception cref="Exception">                    Thrown when an exception error condition
        ///                                                 occurs. </exception>
        public void Load()
        {
            if (this.DllHandle != IntPtr.Zero)
            {
                throw new InvalidOperationException("Dynamic Link Library already loaded.");
            }

            var handle = this.LoadDynamicLibrary(this.Filename);
            if (handle == IntPtr.Zero)
            {
                throw new Exception($"Could not load the dynamic link library '{this.Filename}'.");
            }

            this.DllHandle = handle;
        }

        /// <summary>   Gets the specified procedure's address. </summary>
        /// <param name="functionName"> Name of the function. </param>
        /// <returns>   The procedure's address. </returns>
        public abstract IntPtr GetProcAddress(string functionName);

        /// <summary>   Loads the DLL/dynlib. The returned value is an OS-specific handle, and it is non-zero if successful.</summary>
        /// <param name="filename"> Filename of the file. </param>
        /// <returns>   The OS-specific handle representing the DLL/dynlib (which is non-zero if successful). A zero is indicating an error. </returns>
        protected abstract IntPtr LoadDynamicLibrary(string filename);

        /// <summary> Throw an 'InvalidOperationException' if the instance is not operational (i.e. if 'DllHandle' is null).</summary>
        /// <exception cref="InvalidOperationException"> Thrown when the instance is not operational.</exception>
        protected void ThrowIfNotOperational()
        {
            if (this.DllHandle == IntPtr.Zero)
            {
                throw new InvalidOperationException("Dynamic link library is not loaded.");
            }
        }
    }
}
