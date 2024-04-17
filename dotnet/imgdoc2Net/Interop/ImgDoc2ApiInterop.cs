// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interop
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Runtime.InteropServices;
    using System.Text;
    using ImgDoc2Net.Implementation;
    using ImgDoc2Net.Interfaces;

    /// <summary>   
    /// This class contains "the lowest level interop interface". 
    /// </summary>
    internal partial class ImgDoc2ApiInterop
    {
        private const string BaseDllNameWindows = @"imgdoc2API";
        private const string BaseDllNameLinux = @"libimgdoc2API";

        private static readonly Lazy<ImgDoc2ApiInterop> ImgDoc2ApiInteropInstance = new Lazy<ImgDoc2ApiInterop>(() => new ImgDoc2ApiInterop(), System.Threading.LazyThreadSafetyMode.ExecutionAndPublication);

        /// <summary>   
        /// (Immutable) Win32-Handle of the DLL. If this is IntPtr.Zero, then the loading of the Dll failed (and 'dllLoadErrorMsg' may contain additional information about the problem).
        /// </summary>
        private readonly DllLoader dllLoader;

        private readonly string dllLoadErrorMsg;

        /// <summary>
        /// Prevents a default instance of the <see cref="ImgDoc2ApiInterop"/> class from being created.
        /// </summary>
        /// <remarks>
        /// The strategy of this implementation for dealing with errors (ie. native DLL could not be loaded or some function-exports
        /// could not be found) is:
        /// <list>
        /// <item>
        /// The constructor does NOT throw an exception (ie. the singleton is always available).
        /// </item>
        /// <item>
        /// But, in case of an error-condition, every method will immediately throw an InvalidOperationException exception.
        /// </item>
        /// </list>
        /// </remarks>
        private ImgDoc2ApiInterop()
        {
            var possibleDllFilenames = this.GetFullyQualifiedDllPaths();

            // we try to load all filenames we got, and if "Load" succeeded, we keep this DllLoader-instance
            foreach (var filename in possibleDllFilenames)
            {
                var loader = DllLoader.GetDllLoader(filename);
                try
                {
                    loader.Load();
                    this.dllLoader = loader;
                }
                catch (Exception e)
                {
                    this.dllLoadErrorMsg = e.ToString();
                }
            }

            // it there is no operational DllLoader-instance at this point, we get out of here (and leave this instance in a non-operational state)
            if (this.dllLoader == null)
            {
                return;
            }

            try
            {
                this.getStatistics =
                    this.GetProcAddressThrowIfNotFound<GetStatisticsDelegate>("GetStatistics");
                this.createEnvironmentObject =
                    this.GetProcAddressThrowIfNotFound<CreateEnvironmentObjectDelegate>("CreateEnvironmentObject");
                this.createCreateOptions =
                    this.GetProcAddressThrowIfNotFound<VoidAndReturnIntPtrDelegate>("CreateCreateOptions");
                this.destroyCreateOptions =
                    this.GetProcAddressThrowIfNotFound<IntPtrAndErrorInformationReturnErrorCodeDelegate>("DestroyCreateOptions");
                this.createOpenExistingOptions =
                    this.GetProcAddressThrowIfNotFound<VoidAndReturnIntPtrDelegate>("CreateOpenExistingOptions");
                this.destroyOpenExistingOptions =
                    this.GetProcAddressThrowIfNotFound<IntPtrAndErrorInformationReturnErrorCodeDelegate>("DestroyOpenExistingOptions");
                this.createOptionsSetFilename =
                    this.GetProcAddressThrowIfNotFound<CreateOptionsSetFilenameDelegate>("CreateOptions_SetFilename");
                this.createOptionsGetFilename =
                    this.GetProcAddressThrowIfNotFound<CreateOptionsGetFilenameDelegate>("CreateOptions_GetFilename");
                this.createOptionsGetUseSpatialIndex =
                    this.GetProcAddressThrowIfNotFound<CreateOptions_GetBooleanDelegate>("CreateOptions_GetUseSpatialIndex");
                this.createOptionsSetUseSpatialIndex =
                    this.GetProcAddressThrowIfNotFound<CreateOptions_SetBooleanDelegate>("CreateOptions_SetUseSpatialIndex");
                this.createOptionsGetUseBlobTable =
                    this.GetProcAddressThrowIfNotFound<CreateOptions_GetBooleanDelegate>("CreateOptions_GetUseBlobTable");
                this.createOptionsSetUseBlobTable =
                    this.GetProcAddressThrowIfNotFound<CreateOptions_SetBooleanDelegate>("CreateOptions_SetUseBlobTable");
                this.createOptionsSetDocumentType =
                    this.GetProcAddressThrowIfNotFound<CreateOptions_SetDocumentTypeDelegate>("CreateOptions_SetDocumentType");
                this.createOptionsGetDocumentType =
                    this.GetProcAddressThrowIfNotFound<CreateOptions_GetDocumentTypeDelegate>("CreateOptions_GetDocumentType");
                this.openExistingOptionsSetFilename =
                    this.GetProcAddressThrowIfNotFound<OpenExistingOptionsSetFilenameDelegate>("OpenExistingOptions_SetFilename");
                this.openExistingOptionsGetFilename =
                    this.GetProcAddressThrowIfNotFound<OpenExistingOptionsGetFilenameDelegate>("OpenExistingOptions_GetFilename");

                this.createOptionsAddDimension =
                    this.GetProcAddressThrowIfNotFound<CreateOptions_AddDimensionDelegate>("CreateOptions_AddDimension");
                this.createOptionsAddIndexedDimension =
                    this.GetProcAddressThrowIfNotFound<CreateOptions_AddDimensionDelegate>("CreateOptions_AddIndexedDimension");
                this.createOptionsGetDimensions =
                    this.GetProcAddressThrowIfNotFound<CreateOptions_GetDimensionsDelegate>("CreateOptions_GetDimensions");
                this.createOptionsGetIndexedDimensions =
                    this.GetProcAddressThrowIfNotFound<CreateOptions_GetDimensionsDelegate>("CreateOptions_GetIndexedDimensions");

                this.createNewDocument =
                    this.GetProcAddressThrowIfNotFound<CreateNewDocumentDelegate>("CreateNewDocument");
                this.openExistingDocument =
                    this.GetProcAddressThrowIfNotFound<OpenExistingDocumentDelegate>("OpenExistingDocument");
                this.destroyDocument =
                    this.GetProcAddressThrowIfNotFound<IntPtrAndErrorInformationReturnErrorCodeDelegate>("DestroyDocument");

                this.documentGetReader2d =
                    this.GetProcAddressThrowIfNotFound<IDoc_GetObjectDelegate>("IDoc_GetReader2d");
                this.destroyReader2d =
                    this.GetProcAddressThrowIfNotFound<IntPtrAndErrorInformationReturnErrorCodeDelegate>("DestroyReader2d");
                this.documentGetWriter2d =
                    this.GetProcAddressThrowIfNotFound<IDoc_GetObjectDelegate>("IDoc_GetWriter2d");
                this.destroyWriter2d =
                    this.GetProcAddressThrowIfNotFound<IntPtrAndErrorInformationReturnErrorCodeDelegate>("DestroyWriter2d");
                this.documentGetReader3d =
                    this.GetProcAddressThrowIfNotFound<IDoc_GetObjectDelegate>("IDoc_GetReader3d");
                this.destroyReader3d =
                    this.GetProcAddressThrowIfNotFound<IntPtrAndErrorInformationReturnErrorCodeDelegate>("DestroyReader3d");
                this.documentGetWriter3d =
                    this.GetProcAddressThrowIfNotFound<IDoc_GetObjectDelegate>("IDoc_GetWriter3d");
                this.destroyWriter3d =
                    this.GetProcAddressThrowIfNotFound<IntPtrAndErrorInformationReturnErrorCodeDelegate>("DestroyWriter3d");

                this.idocwrite2dAddTile =
                    this.GetProcAddressThrowIfNotFound<IDocWrite2d_AddTileDelegate>("IDocWrite2d_AddTile");
                this.idocread2dQuery =
                    this.GetProcAddressThrowIfNotFound<IDocRead2d3d_QueryDelegate>("IDocRead2d_Query");
                this.idocread2dGetTilesIntersectingRect =
                    this.GetProcAddressThrowIfNotFound<IDocRead2d_GetTilesIntersectingRectDelegate>("IDocRead2d_GetTilesIntersectingRect");
                this.idocread2dReadTileData =
                    this.GetProcAddressThrowIfNotFound<IDocRead2d_ReadTileDataDelegate>("IDocRead2d_ReadTileData");
                this.idocread2ReadTileInfo =
                    this.GetProcAddressThrowIfNotFound<IDocRead2d_ReadTileInfoDelegate>("IDocRead2d_ReadTileInfo");

                this.idocwrite3dAddBrick =
                    this.GetProcAddressThrowIfNotFound<IDocWrite3d_AddBrickDelegate>("IDocWrite3d_AddBrick");
                this.idocread3dQuery =
                    this.GetProcAddressThrowIfNotFound<IDocRead2d3d_QueryDelegate>("IDocRead3d_Query");
                this.idocread3dGetBricksIntersectingCuboid =
                    this.GetProcAddressThrowIfNotFound<IDocRead3d_GetBricksIntersectingCuboidDelegate>("IDocRead3d_GetBricksIntersectingCuboid");
                this.idocread3dGetBricksIntersectingPlane =
                    this.GetProcAddressThrowIfNotFound<IDocRead3d_GetBricksIntersectingPlaneDelegate>("IDocRead3d_GetBricksIntersectingPlane");
                this.idocread3dReadBrickInfo =
                    this.GetProcAddressThrowIfNotFound<IDocRead3d_ReadBrickInfoDelegate>("IDocRead3d_ReadBrickInfo");
                this.idocread3dReadBrickData =
                    this.GetProcAddressThrowIfNotFound<IDocRead3d_ReadBrickDataDelegate>("IDocRead3d_ReadBrickData");

                this.idocinfo2dGetTileDimensions =
                    this.GetProcAddressThrowIfNotFound<IDocInfo_GetTileDimensionsDelegate>("IDocInfo2d_GetTileDimensions");
                this.idocinfo3dGetTileDimensions =
                    this.GetProcAddressThrowIfNotFound<IDocInfo_GetTileDimensionsDelegate>("IDocInfo3d_GetTileDimensions");
                this.idocinfo2dGetMinMaxForTileDimensions =
                    this.GetProcAddressThrowIfNotFound<IDocInfo_GetMinMaxForTileDimensionsDelegate>("IDocInfo2d_GetMinMaxForTileDimensions");
                this.idocinfo3dGetMinMaxForTileDimensions =
                    this.GetProcAddressThrowIfNotFound<IDocInfo_GetMinMaxForTileDimensionsDelegate>("IDocInfo3d_GetMinMaxForTileDimensions");
                this.idocinfo2dGetBoundingBoxForTiles =
                    this.GetProcAddressThrowIfNotFound<IDocInfo2d_GetBoundingBoxForTilesDelegate>("IDocInfo2d_GetBoundingBoxForTiles");
                this.idocinfo3dGetBoundingBoxForBricks =
                    this.GetProcAddressThrowIfNotFound<IDocInfo3d_GetBoundingBoxForBricksDelegate>("IDocInfo3d_GetBoundingBoxForBricks");
                this.idocinfo2dGetTotalTileCount =
                    this.GetProcAddressThrowIfNotFound<IDocInfo_GetTotalTileCountDelegate>("IDocInfo2d_GetTotalTileCount");
                this.idocinfo3dGetTotalTileCount =
                    this.GetProcAddressThrowIfNotFound<IDocInfo_GetTotalTileCountDelegate>("IDocInfo3d_GetTotalTileCount");
                this.idocinfo2dGetTileCountPerLayer =
                    this.GetProcAddressThrowIfNotFound<IDocInfo_GetTileCountPerLayerDelegate>("IDocInfo2d_GetTileCountPerLayer");
                this.idocinfo3dGetTileCountPerLayer =
                    this.GetProcAddressThrowIfNotFound<IDocInfo_GetTileCountPerLayerDelegate>("IDocInfo3d_GetTileCountPerLayer");

                this.idocwrite2dBeginTransaction = this.GetProcAddressThrowIfNotFound<IDocWrite2d3d_BeginCommitRollbackTransactionDelegate>("IDocWrite2d_BeginTransaction");
                this.idocwrite2dCommitTransaction = this.GetProcAddressThrowIfNotFound<IDocWrite2d3d_BeginCommitRollbackTransactionDelegate>("IDocWrite2d_CommitTransaction");
                this.idocwrite2dRollbackTransaction = this.GetProcAddressThrowIfNotFound<IDocWrite2d3d_BeginCommitRollbackTransactionDelegate>("IDocWrite2d_RollbackTransaction");
                this.idocwrite3dBeginTransaction = this.GetProcAddressThrowIfNotFound<IDocWrite2d3d_BeginCommitRollbackTransactionDelegate>("IDocWrite3d_BeginTransaction");
                this.idocwrite3dCommitTransaction = this.GetProcAddressThrowIfNotFound<IDocWrite2d3d_BeginCommitRollbackTransactionDelegate>("IDocWrite3d_CommitTransaction");
                this.idocwrite3dRollbackTransaction = this.GetProcAddressThrowIfNotFound<IDocWrite2d3d_BeginCommitRollbackTransactionDelegate>("IDocWrite3d_RollbackTransaction");

                this.getVersionInfo =
                    this.GetProcAddressThrowIfNotFound<GetVersionInfoDelegate>("GetVersionInfo");

                this.decodeImage =
                    this.GetProcAddressThrowIfNotFound<DecodeImageDelegate>("DecodeImage");

                // Note: make sure that the delegates used below are not (i.e. NEVER) garbage-collected, otherwise the native code will crash
                this.funcPtrBlobOutputSetSizeForwarder =
                    Marshal.GetFunctionPointerForDelegate<BlobOutputSetSizeDelegate>(ImgDoc2ApiInterop.BlobOutputSetSizeDelegateObj);
                this.funcPtrBlobOutputSetDataForwarder =
                    Marshal.GetFunctionPointerForDelegate<BlobOutputSetDataDelegate>(ImgDoc2ApiInterop.BlobOutputSetDataDelegateObj);
                this.funcPtrAllocateMemoryByteArray =
                    Marshal.GetFunctionPointerForDelegate<AllocateMemoryDelegate>(ImgDoc2ApiInterop.AllocateMemoryByteArrayDelegateObj);

                this.InitializeEnvironmentObject();
            }
            catch (InvalidOperationException exception)
            {
                this.dllLoadErrorMsg = exception.ToString();
                this.dllLoader = null;
            }
        }

        /// <summary>
        /// Gets the (one and only) instance.
        /// </summary>
        /// <value>
        /// The instance.
        /// </value>
        public static ImgDoc2ApiInterop Instance => ImgDoc2ApiInterop.ImgDoc2ApiInteropInstance.Value;

        private static string GetMangledName(string functionName)
        {
            return functionName;
        }

        /// <summary>   
        /// Utility to create a .NET-delegate for the specified exported function. 
        /// If the exported function is not found, then an 'InvalidOperationException' exception is thrown.
        /// </summary>
        /// <typeparam name="T">    The delegate type definition. </typeparam>
        /// <param name="name" type="string">   The name of the exported function. </param>
        /// <returns type="T">  
        /// The newly constructed delegate.
        /// </returns>
        private T GetProcAddressThrowIfNotFound<T>(string name)
            where T : Delegate
        {
            T del = this.TryGetProcAddress<T>(name);
            if (del == null)
            {
                throw new InvalidOperationException($"Function \"{name}\" was not found.");
            }

            return del;
        }

        private T TryGetProcAddress<T>(string name)
            where T : Delegate
        {
            IntPtr addressOfFunctionToCall = this.dllLoader.GetProcAddress(GetMangledName(name));
            if (addressOfFunctionToCall == IntPtr.Zero)
            {
                return null;
            }

            return (T)Marshal.GetDelegateForFunctionPointer(addressOfFunctionToCall, typeof(T));
        }

        private void ThrowIfNotInitialized()
        {
            if (this.dllLoader == null)
            {
                throw new InvalidOperationException(
                    $"{nameof(ImgDoc2ApiInterop)} is not operational, native dynamic-link-library could not be loaded. Debug-info: {this.dllLoadErrorMsg ?? "<not available>"}");
            }
        }

        private IEnumerable<string> GetFullyQualifiedDllPaths()
        {
            // Note: We are looking for the native DLL in the following locations:
            // - in the same directory as the executing assembly
            // - in the "runtimes/xxx/native" subdirectory of the executing assembly, where "xxx" is the runtime identifier (c.f. https://learn.microsoft.com/en-us/dotnet/core/rid-catalog)
            //
            // Currently, we only support linux-x64, win-x64 and win-arm64.
            // The Nuget-package contains the native DLLs in the "runtimes" directory. Searching in the same directory is used for development purposes.
            // 
            // TODO(JBL): I'd like to have CPU-architecture-specific suffixes for the filenames ("x86", "x64", "arm32" etc.) and probably a "d" for debug-builds or so
            string pathOfExecutable = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location) ?? string.Empty;
            bool isLinux = Utilities.IsLinux();

            string filenameOfBinary;
            if (!isLinux)
            {
                filenameOfBinary = BaseDllNameWindows + ".dll";
            }
            else
            {
                filenameOfBinary = BaseDllNameLinux + ".so";
            }

            yield return Path.Combine(pathOfExecutable, filenameOfBinary);

            if (isLinux)
            {
                yield return Path.Combine(pathOfExecutable, "runtimes/linux-x64/native/" + filenameOfBinary);
            }
            else
            {
                if (Utilities.IsCpuArchitectureX64())
                {
                    yield return Path.Combine(pathOfExecutable, "runtimes/win-x64/native/" + filenameOfBinary);
                }
                else if (Utilities.IsCpuArchitectureArm64())
                {
                    yield return Path.Combine(pathOfExecutable, "runtimes/win-arm64/native/" + filenameOfBinary);
                }
            }
        }
    }

    /// <content>   
    /// In this part we find the wrappers for calling into native code. 
    /// </content>
    internal partial class ImgDoc2ApiInterop
    {
        /// <summary>
        /// Retrieves the version information of the native library.
        /// </summary>
        /// <returns>The version information of the native library.</returns>
        public ImgDoc2NativeLibraryVersionInfo GetNativeLibraryVersionInfo()
        {
            this.ThrowIfNotInitialized();

            unsafe
            {
                VersionInfoInterop versionInfoInterop = default(VersionInfoInterop);

                try
                {
                    int returnCode = this.getVersionInfo(&versionInfoInterop, this.funcPtrAllocateMemoryByteArray);
                    if (returnCode != ImgDoc2_ErrorCode_OK)
                    {
                        throw new InvalidOperationException($"Error in getVersionInfo: {returnCode}");
                    }

                    return new ImgDoc2NativeLibraryVersionInfo()
                    {
                        Major = versionInfoInterop.Major,
                        Minor = versionInfoInterop.Minor,
                        Patch = versionInfoInterop.Patch,
                        CompilerIdentification = Utilities.ConvertFromUtf8Span(ImgDoc2ApiInterop.ConvertAllocationObjectToByteArrayAndFreeGcHandle(ref versionInfoInterop.CompilerIdentification)),
                        BuildType = Utilities.ConvertFromUtf8Span(ImgDoc2ApiInterop.ConvertAllocationObjectToByteArrayAndFreeGcHandle(ref versionInfoInterop.BuildType)),
                        RepositoryUrl = Utilities.ConvertFromUtf8Span(ImgDoc2ApiInterop.ConvertAllocationObjectToByteArrayAndFreeGcHandle(ref versionInfoInterop.RepositoryUrl)),
                        RepositoryBranch = Utilities.ConvertFromUtf8Span(ImgDoc2ApiInterop.ConvertAllocationObjectToByteArrayAndFreeGcHandle(ref versionInfoInterop.RepositoryBranch)),
                        RepositoryTag = Utilities.ConvertFromUtf8Span(ImgDoc2ApiInterop.ConvertAllocationObjectToByteArrayAndFreeGcHandle(ref versionInfoInterop.RepositoryTag)),
                    };
                }
                finally
                {
                    // Note: The un-pinning of the byte-arrays is done in the ConvertAllocationObjectToByteArrayAndFreeGcHandle-method already,
                    //        so this under normal circumstances should not be necessary (it is just a safety-net e.g. if an exception is thrown).
                    ImgDoc2ApiInterop.FreeAllocationObject(ref versionInfoInterop.CompilerIdentification);
                    ImgDoc2ApiInterop.FreeAllocationObject(ref versionInfoInterop.BuildType);
                    ImgDoc2ApiInterop.FreeAllocationObject(ref versionInfoInterop.RepositoryUrl);
                    ImgDoc2ApiInterop.FreeAllocationObject(ref versionInfoInterop.RepositoryBranch);
                    ImgDoc2ApiInterop.FreeAllocationObject(ref versionInfoInterop.RepositoryTag);
                }
            }
        }

        /// <summary>   Gets the "ImgDoc2Statistics" - information about how many active objects exist. </summary>
        /// <returns>   The "ImgDoc2Statistics". </returns>
        public ImgDoc2Statistics GetStatistics()
        {
            this.ThrowIfNotInitialized();

            unsafe
            {
                ImgDoc2StatisticsInterop statisticsInterop = default(ImgDoc2StatisticsInterop);
                this.getStatistics(&statisticsInterop);

                return new ImgDoc2Statistics()
                {
                    NumberOfCreateOptionsObjectsActive = (int)statisticsInterop.NumberOfCreateOptionsObjectsActive,
                    NumberOfOpenExistingOptionsObjectsActive = (int)statisticsInterop.NumberOfOpenExistingOptionsObjectsActive,
                    NumberOfDocumentObjectsActive = (int)statisticsInterop.NumberOfDocumentObjectsActive,
                    NumberOfReader2dObjectsActive = (int)statisticsInterop.NumberOfReader2dObjectsActive,
                    NumberOfWriter2dObjectsActive = (int)statisticsInterop.NumberOfWriter2dObjectsActive,
                    NumberOfReader3dObjectsActive = (int)statisticsInterop.NumberOfReader3dObjectsActive,
                    NumberOfWriter3dObjectsActive = (int)statisticsInterop.NumberOfWriter3dObjectsActive,
                };
            }
        }

        /// <summary> Creates a new "create-options" object, represented by a handle.</summary>
        /// <returns> The handle of the newly created "create-options" object.</returns>
        public IntPtr CreateCreateOptions()
        {
            this.ThrowIfNotInitialized();
            return this.createCreateOptions();
        }

        /// <summary> Destroys the specified "create-options" object.</summary>
        /// <param name="handleCreateOptions"> The handle of the "create-options" object to be destroyed.</param>
        public void DestroyCreateOptions(IntPtr handleCreateOptions)
        {
            this.ThrowIfNotInitialized();

            unsafe
            {
                ImgDoc2ErrorInformation errorInformation;
                int returnCode = this.destroyCreateOptions(handleCreateOptions, &errorInformation);
                this.HandleErrorCases(returnCode, in errorInformation);
            }
        }

        /// <summary> Set the filename property of the specified "create-options" object.</summary>
        /// <param name="handleCreateOptions"> The handle of the "create-options" object to be modified.</param>
        /// <param name="filename">            Filename of the file.</param>
        public void CreateOptionsSetFilename(IntPtr handleCreateOptions, string filename)
        {
            this.ThrowIfNotInitialized();
            if (filename == null)
            {
                throw new ArgumentNullException(nameof(filename), "Must not be null.");
            }

            byte[] bytesUtf8 = Encoding.UTF8.GetBytes(filename);
            unsafe
            {
                fixed (byte* pointerBytesUtf8 = &bytesUtf8[0])
                {
                    this.createOptionsSetFilename(handleCreateOptions, new IntPtr(pointerBytesUtf8), null);
                }
            }
        }

        /// <summary>   Get the filename property of the specified "create-options" object.</summary>
        /// <param name="handleCreateOptions">  The handle of the "create-options" object to be queried. </param>
        /// <returns>   The filename property. </returns>
        public string CreateOptionsGetFilename(IntPtr handleCreateOptions)
        {
            this.ThrowIfNotInitialized();
            return this.GetStringInteropHelper(
                (IntPtr fileNameUtf8, IntPtr size) =>
                {
                    unsafe
                    {
                        return this.createOptionsGetFilename(handleCreateOptions, fileNameUtf8, size, null);
                    }
                },
                nameof(this.CreateOptionsGetFilename));
        }

        /// <summary> Method operating on a CreateOptions-object: set the document type.</summary>
        /// <param name="handleCreateOptions"> The handle of the CreateOptions object.</param>
        /// <param name="documentType">        Type of the document.</param>
        public void CreateOptionsSetDocumentType(IntPtr handleCreateOptions, DocumentType documentType)
        {
            this.ThrowIfNotInitialized();
            ImgDoc2ErrorInformation errorInformation;
            int returnCode;
            unsafe
            {
                returnCode = this.createOptionsSetDocumentType(handleCreateOptions, (byte)documentType, &errorInformation);
            }

            this.HandleErrorCases(returnCode, in errorInformation);
        }

        /// <summary> Method operating on a CreateOptions-object: set the document type.</summary>
        /// <param name="handleCreateOptions"> The handle of the CreateOptions object.</param>
        /// <returns> The document type.</returns>
        public DocumentType CreateOptionsGetDocumentType(IntPtr handleCreateOptions)
        {
            this.ThrowIfNotInitialized();
            byte documentType;
            ImgDoc2ErrorInformation errorInformation;
            int returnCode;
            unsafe
            {
                returnCode = this.createOptionsGetDocumentType(handleCreateOptions, &documentType, &errorInformation);
            }

            this.HandleErrorCases(returnCode, in errorInformation);
            return (DocumentType)Enum.ToObject(typeof(DocumentType), documentType);
        }

        public bool CreateOptionsGetUseSpatialIndex(IntPtr handleCreateOptions)
        {
            this.ThrowIfNotInitialized();
            bool useSpatialIndex;
            int returnCode;
            ImgDoc2ErrorInformation errorInformation;
            unsafe
            {
                returnCode = this.createOptionsGetUseSpatialIndex(handleCreateOptions, &useSpatialIndex, &errorInformation);
            }

            this.HandleErrorCases(returnCode, in errorInformation);
            return useSpatialIndex;
        }

        public bool CreateOptionsGetUseBlobTable(IntPtr handleCreateOptions)
        {
            this.ThrowIfNotInitialized();
            bool useBlobTable;
            int returnCode;
            ImgDoc2ErrorInformation errorInformation;
            unsafe
            {
                returnCode = this.createOptionsGetUseBlobTable(handleCreateOptions, &useBlobTable, &errorInformation);
            }

            this.HandleErrorCases(returnCode, in errorInformation);
            return useBlobTable;
        }

        public void CreateOptionsSetUseSpatialIndex(IntPtr handleCreateOptions, bool useSpatialIndex)
        {
            this.ThrowIfNotInitialized();
            int returnCode;
            ImgDoc2ErrorInformation errorInformation;
            unsafe
            {
                returnCode = this.createOptionsSetUseSpatialIndex(handleCreateOptions, useSpatialIndex, &errorInformation);
            }

            this.HandleErrorCases(returnCode, in errorInformation);
        }

        public void CreateOptionsSetUseBlobTable(IntPtr handleCreateOptions, bool useBlobTable)
        {
            this.ThrowIfNotInitialized();
            int returnCode;
            ImgDoc2ErrorInformation errorInformation;
            unsafe
            {
                returnCode = this.createOptionsSetUseBlobTable(handleCreateOptions, useBlobTable, &errorInformation);
            }

            this.HandleErrorCases(returnCode, in errorInformation);
        }

        /// <summary> Method operating on a CreateOptions-object: Add a dimension.</summary>
        /// <param name="handleCreateOptions"> The handle of the "create-options" object.</param>
        /// <param name="dimension">           The dimension to be added.</param>
        public void CreateOptionsAddDimension(IntPtr handleCreateOptions, Dimension dimension)
        {
            this.ThrowIfNotInitialized();
            int returnCode;
            ImgDoc2ErrorInformation errorInformation;
            unsafe
            {
                returnCode = this.createOptionsAddDimension(handleCreateOptions, (byte)dimension.Id, &errorInformation);
            }

            this.HandleErrorCases(returnCode, in errorInformation);
        }

        /// <summary> Method operating on a CreateOptions-object: Specify a dimension for which an index is to be created..</summary>
        /// <param name="handleCreateOptions"> The handle of the "create-options" object.</param>
        /// <param name="dimension">           The dimension.</param>
        public void CreateOptionsAddIndexedDimension(IntPtr handleCreateOptions, Dimension dimension)
        {
            this.ThrowIfNotInitialized();
            int returnCode;
            ImgDoc2ErrorInformation errorInformation;
            unsafe
            {
                returnCode = this.createOptionsAddIndexedDimension(handleCreateOptions, (byte)dimension.Id, null);
            }

            this.HandleErrorCases(returnCode, in errorInformation);
        }

        /// <summary> Method operating on a CreateOptions-object: Get list of dimensions.</summary>
        /// <param name="handleCreateOptions"> The handle of the "create-options" object.</param>
        /// <returns> An array containing the dimensions.</returns>
        public Dimension[] CreateOptionsGetDimensions(IntPtr handleCreateOptions)
        {
            this.ThrowIfNotInitialized();

            ImgDoc2ErrorInformation errorInformation;

            // for sake of simplicity, we only call once into native code with a buffer which is large enough for
            //  all conceivable cases
            const int bufferSize = 64;
            unsafe
            {
                byte* dimensionsBuffer = stackalloc byte[bufferSize];

                // TODO(Jbl):  we are abusing UIntPtr as an equivalent to size_t, c.f. https://stackoverflow.com/questions/32906774/what-is-equal-to-the-c-size-t-in-c-sharp
                UIntPtr sizeOfBuffer = new UIntPtr(bufferSize);
                var returnCode = this.createOptionsGetDimensions(handleCreateOptions, dimensionsBuffer, new IntPtr(&sizeOfBuffer), null);
                if (sizeOfBuffer.ToUInt32() > bufferSize)
                {
                    throw new NotImplementedException("Buffersize exceeded, may want to implement variable buffersizes.");
                }

                this.HandleErrorCases(returnCode, in errorInformation);

                Dimension[] dimensions = new Dimension[sizeOfBuffer.ToUInt32()];
                for (int i = 0; i < sizeOfBuffer.ToUInt32(); i++)
                {
                    dimensions[i] = new Dimension((char)dimensionsBuffer[i]);
                }

                return dimensions;
            }
        }

        /// <summary> Method operating on a CreateOptions-object: Get list of dimensions for which an index is to be created.</summary>
        /// <param name="handleCreateOptions"> The handle of the "create-options" object.</param>
        /// <returns> An array containing the dimensions for which an index is to be created.</returns>
        public Dimension[] CreateOptionsGetIndexedDimensions(IntPtr handleCreateOptions)
        {
            this.ThrowIfNotInitialized();

            ImgDoc2ErrorInformation errorInformation;

            // for sake of simplicity, we only call once into native code with a buffer which is large enough for
            //  all conceivable cases
            const int bufferSize = 64;
            unsafe
            {
                byte* dimensionsBuffer = stackalloc byte[bufferSize];
                UIntPtr sizeOfBuffer = new UIntPtr(bufferSize); // TODO(Jbl):  we are abusing UIntPtr as an equivalent to size_t, c.f. https://stackoverflow.com/questions/32906774/what-is-equal-to-the-c-size-t-in-c-sharp
                int returnCode = this.createOptionsGetIndexedDimensions(handleCreateOptions, dimensionsBuffer, new IntPtr(&sizeOfBuffer), null);
                if (sizeOfBuffer.ToUInt32() > bufferSize)
                {
                    throw new NotImplementedException("Buffersize exceeded, may want to implement variable buffersizes.");
                }

                this.HandleErrorCases(returnCode, in errorInformation);

                Dimension[] dimensions = new Dimension[sizeOfBuffer.ToUInt32()];
                for (int i = 0; i < sizeOfBuffer.ToUInt32(); i++)
                {
                    dimensions[i] = new Dimension((char)dimensionsBuffer[i]);
                }

                return dimensions;
            }
        }

        /// <summary> Creates a new 'OpenExistingOptions' object.</summary>
        /// <returns> A handle representing the newly create 'OpenExistingOptions' object.</returns>
        public IntPtr CreateOpenExistingOptions()
        {
            this.ThrowIfNotInitialized();
            return this.createOpenExistingOptions();
        }

        /// <summary> Destroys the 'OpenExistingOptions' object.</summary>
        /// <param name="handleOpenExistingOptions"> The handle of the 'OpenExistingOptions' object to be destroyed.</param>
        public void DestroyOpenExistingOptions(IntPtr handleOpenExistingOptions)
        {
            this.ThrowIfNotInitialized();

            unsafe
            {
                ImgDoc2ErrorInformation errorInformation;
                int returnCode = this.destroyOpenExistingOptions(handleOpenExistingOptions, &errorInformation);
                this.HandleErrorCases(returnCode, in errorInformation);
            }
        }

        /// <summary> Method operating on an OpenExistingOptions-object: set the filename.</summary>
        /// <exception cref="ArgumentNullException"> Thrown when one or more required arguments are null.</exception>
        /// <param name="handleOpenExistingOptions"> The handle of the 'OpenExistingOptions' object.</param>
        /// <param name="filename">                  The filename.</param>
        public void OpenExistingOptionsSetFilename(IntPtr handleOpenExistingOptions, string filename)
        {
            this.ThrowIfNotInitialized();
            if (filename == null)
            {
                throw new ArgumentNullException(nameof(filename), "Must not be null.");
            }

            byte[] bytesUtf8 = Encoding.UTF8.GetBytes(filename);
            unsafe
            {
                fixed (byte* pointerBytesUtf8 = &bytesUtf8[0])
                {
                    this.openExistingOptionsSetFilename(handleOpenExistingOptions, new IntPtr(pointerBytesUtf8), null);
                }
            }
        }

        /// <summary> Method operating on an OpenExistingOptions-object:  get the filename.</summary>
        /// <param name="handleOpenExistingOptions"> The handle of the 'OpenExistingOptions' object.</param>
        /// <returns> The filename.</returns>
        public string OpenExistingOptionsGetFilename(IntPtr handleOpenExistingOptions)
        {
            return this.GetStringInteropHelper(
                (IntPtr fileNameUtf8, IntPtr size) =>
                {
                    unsafe
                    {
                        return this.openExistingOptionsGetFilename(handleOpenExistingOptions, fileNameUtf8, size, null);
                    }
                },
                "OpenExistingOptionsGetFilename");
        }

        /// <summary> Creates new document using the specified "CreateOptions"-object.</summary>
        /// <param name="handleCreateOptions"> The handle of the "create-options" object to be used describing the operation.</param>
        /// <returns> The handle representing the newly created document.</returns>
        public IntPtr CreateNewDocument(IntPtr handleCreateOptions)
        {
            this.ThrowIfNotInitialized();

            int returnCode;
            IntPtr documentHandle;
            ImgDoc2ErrorInformation errorInformation;
            unsafe
            {
                returnCode = this.createNewDocument(handleCreateOptions, this.environmentObjectHandle, &documentHandle, &errorInformation);
            }

            this.HandleErrorCases(returnCode, in errorInformation);
            return documentHandle;
        }

        /// <summary>   Creates new document using the specified "OpenExistingOptions"-object.</summary>
        /// <param name="handleOpenExistingOptions">    Options for controlling the operation. </param>
        /// <returns>   The handle representing the newly created document.</returns>
        public IntPtr OpenExistingDocument(IntPtr handleOpenExistingOptions)
        {
            this.ThrowIfNotInitialized();

            int returnCode;
            IntPtr documentHandle;
            ImgDoc2ErrorInformation errorInformation;
            unsafe
            {
                returnCode = this.openExistingDocument(handleOpenExistingOptions, this.environmentObjectHandle, &documentHandle, &errorInformation);
            }

            this.HandleErrorCases(returnCode, in errorInformation);
            return documentHandle;
        }

        /// <summary> Destroys the document described by handleDocument.</summary>
        /// <param name="handleDocument"> The handle of the document-object.</param>
        public void DestroyDocument(IntPtr handleDocument)
        {
            this.ThrowIfNotInitialized();

            unsafe
            {
                ImgDoc2ErrorInformation errorInformation;
                int returnCode = this.destroyDocument(handleDocument, &errorInformation);
                this.HandleErrorCases(returnCode, in errorInformation);
            }
        }

        public IntPtr DocumentGetReader2d(IntPtr handleDocument)
        {
            this.ThrowIfNotInitialized();

            int returnCode;
            IntPtr reader2dHandle;
            ImgDoc2ErrorInformation errorInformation;
            unsafe
            {
                returnCode = this.documentGetReader2d(handleDocument, &reader2dHandle, &errorInformation);
            }

            this.HandleErrorCases(returnCode, in errorInformation);
            return reader2dHandle;
        }

        public void DestroyReader2d(IntPtr handleReader)
        {
            this.ThrowIfNotInitialized();

            unsafe
            {
                ImgDoc2ErrorInformation errorInformation;
                int returnCode = this.destroyReader2d(handleReader, &errorInformation);
                this.HandleErrorCases(returnCode, in errorInformation);
            }
        }

        public IntPtr DocumentGetWriter2d(IntPtr handleDocument)
        {
            this.ThrowIfNotInitialized();

            int returnCode;
            IntPtr writerHandle;
            ImgDoc2ErrorInformation errorInformation;
            unsafe
            {
                returnCode = this.documentGetWriter2d(handleDocument, &writerHandle, &errorInformation);
            }

            this.HandleErrorCases(returnCode, in errorInformation);
            return writerHandle;
        }

        public void DestroyWriter2d(IntPtr handleWriter)
        {
            this.ThrowIfNotInitialized();

            unsafe
            {
                ImgDoc2ErrorInformation errorInformation;
                int returnCode = this.destroyWriter2d(handleWriter, &errorInformation);
                this.HandleErrorCases(returnCode, in errorInformation);
            }
        }

        /// <summary> Create a document3D-reader-object handle 3D.</summary>
        /// <param name="handleDocument"> The handle of the document3D-object.</param>
        /// <returns> A handle to a newly created document3D-reader-object if successful.</returns>
        public IntPtr DocumentGetReader3d(IntPtr handleDocument)
        {
            this.ThrowIfNotInitialized();

            int returnCode;
            IntPtr reader3dHandle;
            ImgDoc2ErrorInformation errorInformation;
            unsafe
            {
                returnCode = this.documentGetReader3d(handleDocument, &reader3dHandle, &errorInformation);
            }

            this.HandleErrorCases(returnCode, in errorInformation);
            return reader3dHandle;
        }

        /// <summary> Destroys the specified document3D-reader-object.</summary>
        /// <param name="handleReader"> The document3D-reader-object.</param>
        public void DestroyReader3d(IntPtr handleReader)
        {
            this.ThrowIfNotInitialized();

            unsafe
            {
                ImgDoc2ErrorInformation errorInformation;
                int returnCode = this.destroyReader3d(handleReader, &errorInformation);
                this.HandleErrorCases(returnCode, in errorInformation);
            }
        }

        public IntPtr DocumentGetWriter3d(IntPtr handleDocument)
        {
            this.ThrowIfNotInitialized();

            int returnCode;
            IntPtr writer3dHandle;
            ImgDoc2ErrorInformation errorInformation;
            unsafe
            {
                returnCode = this.documentGetWriter3d(handleDocument, &writer3dHandle, &errorInformation);
            }

            this.HandleErrorCases(returnCode, in errorInformation);
            return writer3dHandle;
        }

        public void DestroyWriter3d(IntPtr handleWriter)
        {
            this.ThrowIfNotInitialized();

            unsafe
            {
                ImgDoc2ErrorInformation errorInformation;
                int returnCode = this.destroyWriter3d(handleWriter, &errorInformation);
                this.HandleErrorCases(returnCode, in errorInformation);
            }
        }

        public void Writer2dBeginTransaction(IntPtr handle2dWriter)
        {
            this.InternalWriter2d3dTransaction(handle2dWriter, this.idocwrite2dBeginTransaction);
        }

        public void Writer2dCommitTransaction(IntPtr handle2dWriter)
        {
            this.InternalWriter2d3dTransaction(handle2dWriter, this.idocwrite2dCommitTransaction);
        }

        public void Writer2dRollbackTransaction(IntPtr handle2dWriter)
        {
            this.InternalWriter2d3dTransaction(handle2dWriter, this.idocwrite2dRollbackTransaction);
        }

        public long Writer2dAddTile(
            IntPtr write2dHandle,
            ITileCoordinate coordinate,
            in LogicalPosition logicalPosition,
            Tile2dBaseInfo tile2dBaseInfo,
            DataType dataType,
            IntPtr pointerData,
            long dataSize)
        {
            this.ThrowIfNotInitialized();
            byte[] tileCoordinateInterop = ConvertToTileCoordinateInterop(coordinate);
            LogicalPositionInfoInterop logicalPositionInfoInterop = new LogicalPositionInfoInterop(in logicalPosition);
            TileBaseInfoInterop tileBaseInfoInterop = ConvertToTileBaseInfoInterop(tile2dBaseInfo);

            int returnCode;
            ImgDoc2ErrorInformation errorInformation;
            long resultPk;

            unsafe
            {
                fixed (byte* pointerTileCoordinateInterop = &tileCoordinateInterop[0])
                {
                    returnCode = this.idocwrite2dAddTile(
                        write2dHandle,
                        new IntPtr(pointerTileCoordinateInterop),
                        &logicalPositionInfoInterop,
                        &tileBaseInfoInterop,
                        (byte)dataType,
                        pointerData,
                        dataSize,
                        &resultPk,
                        &errorInformation);
                }
            }

            this.HandleErrorCases(returnCode, in errorInformation);
            return resultPk;
        }

        public long Writer3dAddBrick(
            IntPtr write3dHandle,
            ITileCoordinate coordinate,
            in LogicalPosition3d logicalPosition3d,
            Brick3dBaseInfo brick3dBaseInfo,
            DataType dataType,
            IntPtr pointerData,
            long dataSize)
        {
            this.ThrowIfNotInitialized();
            byte[] tileCoordinateInterop = ConvertToTileCoordinateInterop(coordinate);
            LogicalPositionInfo3DInterop logicalPosition3dInfoInterop = new LogicalPositionInfo3DInterop(in logicalPosition3d);
            BrickBaseInfoInterop brickBaseInfoInterop = ConvertToBrickBaseInfoInterop(brick3dBaseInfo);

            int returnCode;
            ImgDoc2ErrorInformation errorInformation;
            long resultPk;

            unsafe
            {
                fixed (byte* pointerTileCoordinateInterop = &tileCoordinateInterop[0])
                {
                    returnCode = this.idocwrite3dAddBrick(
                        write3dHandle,
                        new IntPtr(pointerTileCoordinateInterop),
                        &logicalPosition3dInfoInterop,
                        &brickBaseInfoInterop,
                        (byte)dataType,
                        pointerData,
                        dataSize,
                        &resultPk,
                        &errorInformation);
                }
            }

            this.HandleErrorCases(returnCode, in errorInformation);
            return resultPk;
        }

        public void Writer3dBeginTransaction(IntPtr handle3dWriter)
        {
            this.InternalWriter2d3dTransaction(handle3dWriter, this.idocwrite3dBeginTransaction);
        }

        public void Writer3dCommitTransaction(IntPtr handle3dWriter)
        {
            this.InternalWriter2d3dTransaction(handle3dWriter, this.idocwrite3dCommitTransaction);
        }

        public void Writer3dRollbackTransaction(IntPtr handle3dWriter)
        {
            this.InternalWriter2d3dTransaction(handle3dWriter, this.idocwrite3dRollbackTransaction);
        }

        public QueryResult Reader2dQuery(IntPtr read2dHandle, IDimensionQueryClause clause, ITileInfoQueryClause tileInfoQueryClause, int maxNumberOfResults)
        {
            return this.InternalReaderQuery(this.idocread2dQuery, read2dHandle, clause, tileInfoQueryClause, maxNumberOfResults);
        }

        public QueryResult Reader3dQuery(IntPtr read3dHandle, IDimensionQueryClause clause, ITileInfoQueryClause tileInfoQueryClause, int maxNumberOfResults)
        {
            return this.InternalReaderQuery(this.idocread3dQuery, read3dHandle, clause, tileInfoQueryClause, maxNumberOfResults);
        }

        /// <summary>   Execute a "spatial query" - which may also include a dimension-query-clause and a tile-info-query-clause. </summary>
        /// <param name="read2dHandle" type="IntPtr">                           The read2d-object. </param>
        /// <param name="rectangle" type="Rectangle">                           The query rectangle. </param>
        /// <param name="dimensionQueryClause" type="IDimensionQueryClause">    The dimension query clause (may be null). </param>
        /// <param name="tileInfoQueryClause" type="ITileInfoQueryClause">      The tile information query clause (may be null). </param>
        /// <param name="maxNumberOfResults" type="int">                        The max number of results. </param>
        /// <returns type="QueryResult">    The result of the query. </returns>
        public QueryResult Reader2dQueryTilesIntersectingRect(
            IntPtr read2dHandle,
            Rectangle rectangle,
            IDimensionQueryClause dimensionQueryClause,
            ITileInfoQueryClause tileInfoQueryClause,
            int maxNumberOfResults)
        {
            this.ThrowIfNotInitialized();
            byte[] dimensionQueryClauseInterop = dimensionQueryClause != null
                ? ConvertToTileCoordinateInterop(dimensionQueryClause)
                : null;
            byte[] tileInfoQueryClauseInterop = (tileInfoQueryClause != null)
                ? ConvertToTileInfoQueryInterop(tileInfoQueryClause)
                : null;
            byte[] queryResultInterop = CreateQueryResultInterop(maxNumberOfResults);

            int returnCode;
            ImgDoc2ErrorInformation errorInformation;

            unsafe
            {
                fixed (byte* pointerQueryResultInterop = &queryResultInterop[0])
                {
                    // TODO(JBL): this code is quite lame, maybe there is a better way which prevents us from having to code all those cases...
                    if (dimensionQueryClauseInterop == null && tileInfoQueryClauseInterop == null)
                    {
                        RectangleDoubleInterop rectangleDoubleInterop = new RectangleDoubleInterop()
                        { X = rectangle.X, Y = rectangle.Y, Width = rectangle.Width, Height = rectangle.Height };
                        returnCode = this.idocread2dGetTilesIntersectingRect(
                            read2dHandle,
                            &rectangleDoubleInterop,
                            IntPtr.Zero,
                            IntPtr.Zero,
                            new IntPtr(pointerQueryResultInterop),
                            &errorInformation);
                    }
                    else if (dimensionQueryClauseInterop != null && tileInfoQueryClauseInterop == null)
                    {
                        fixed (byte* pointerDimensionQueryClauseInterop = &dimensionQueryClauseInterop[0])
                        {
                            RectangleDoubleInterop rectangleDoubleInterop = new RectangleDoubleInterop()
                            { X = rectangle.X, Y = rectangle.Y, Width = rectangle.Width, Height = rectangle.Height };
                            returnCode = this.idocread2dGetTilesIntersectingRect(
                                read2dHandle,
                                &rectangleDoubleInterop,
                                new IntPtr(pointerDimensionQueryClauseInterop),
                                IntPtr.Zero,
                                new IntPtr(pointerQueryResultInterop),
                                &errorInformation);
                        }
                    }
                    else if (dimensionQueryClauseInterop == null && tileInfoQueryClauseInterop != null)
                    {
                        fixed (byte* pointerTileInfoQueryClause = &tileInfoQueryClauseInterop[0])
                        {
                            RectangleDoubleInterop rectangleDoubleInterop = new RectangleDoubleInterop()
                            { X = rectangle.X, Y = rectangle.Y, Width = rectangle.Width, Height = rectangle.Height };
                            returnCode = this.idocread2dGetTilesIntersectingRect(
                                read2dHandle,
                                &rectangleDoubleInterop,
                                IntPtr.Zero,
                                new IntPtr(pointerTileInfoQueryClause),
                                new IntPtr(pointerQueryResultInterop),
                                &errorInformation);
                        }
                    }
                    else
                    {
                        // if (dimensionQueryClauseInterop != null && tileInfoQueryClauseInterop != null)
                        fixed (byte* pointerDimensionQueryClauseInterop = &dimensionQueryClauseInterop[0])
                        fixed (byte* pointerTileInfoQueryClause = &tileInfoQueryClauseInterop[0])
                        {
                            RectangleDoubleInterop rectangleDoubleInterop = new RectangleDoubleInterop()
                            { X = rectangle.X, Y = rectangle.Y, Width = rectangle.Width, Height = rectangle.Height };
                            returnCode = this.idocread2dGetTilesIntersectingRect(
                                read2dHandle,
                                &rectangleDoubleInterop,
                                new IntPtr(pointerDimensionQueryClauseInterop),
                                new IntPtr(pointerTileInfoQueryClause),
                                new IntPtr(pointerQueryResultInterop),
                                &errorInformation);
                        }
                    }
                }
            }

            this.HandleErrorCases(returnCode, in errorInformation);

            QueryResult result = ConvertToQueryResult(queryResultInterop);
            return result;
        }

        public QueryResult Reader3dQueryBricksIntersectingCuboid(
            IntPtr read3dHandle,
            Cuboid cuboid,
            IDimensionQueryClause dimensionQueryClause,
            ITileInfoQueryClause tileInfoQueryClause,
            int maxNumberOfResults)
        {
            this.ThrowIfNotInitialized();
            byte[] dimensionQueryClauseInterop = dimensionQueryClause != null
                ? ConvertToTileCoordinateInterop(dimensionQueryClause)
                : null;
            byte[] tileInfoQueryClauseInterop = (tileInfoQueryClause != null)
                ? ConvertToTileInfoQueryInterop(tileInfoQueryClause)
                : null;
            byte[] queryResultInterop = CreateQueryResultInterop(maxNumberOfResults);

            int returnCode;
            ImgDoc2ErrorInformation errorInformation;

            unsafe
            {
                fixed (byte* pointerQueryResultInterop = &queryResultInterop[0])
                {
                    // TODO(JBL): this code is quite lame, maybe there is a better way which prevents us from having to code all those cases...
                    if (dimensionQueryClauseInterop == null && tileInfoQueryClauseInterop == null)
                    {
                        CuboidDoubleInterop cuboidDoubleInterop = new CuboidDoubleInterop()
                        { X = cuboid.X, Y = cuboid.Y, Z = cuboid.Z, Width = cuboid.Width, Height = cuboid.Height, Depth = cuboid.Depth };
                        returnCode = this.idocread3dGetBricksIntersectingCuboid(
                            read3dHandle,
                            &cuboidDoubleInterop,
                            IntPtr.Zero,
                            IntPtr.Zero,
                            new IntPtr(pointerQueryResultInterop),
                            &errorInformation);
                    }
                    else if (dimensionQueryClauseInterop != null && tileInfoQueryClauseInterop == null)
                    {
                        fixed (byte* pointerDimensionQueryClauseInterop = &dimensionQueryClauseInterop[0])
                        {
                            CuboidDoubleInterop cuboidDoubleInterop = new CuboidDoubleInterop()
                            { X = cuboid.X, Y = cuboid.Y, Z = cuboid.Z, Width = cuboid.Width, Height = cuboid.Height, Depth = cuboid.Depth };
                            returnCode = this.idocread3dGetBricksIntersectingCuboid(
                                read3dHandle,
                                &cuboidDoubleInterop,
                                new IntPtr(pointerDimensionQueryClauseInterop),
                                IntPtr.Zero,
                                new IntPtr(pointerQueryResultInterop),
                                &errorInformation);
                        }
                    }
                    else if (dimensionQueryClauseInterop == null && tileInfoQueryClauseInterop != null)
                    {
                        fixed (byte* pointerTileInfoQueryClause = &tileInfoQueryClauseInterop[0])
                        {
                            CuboidDoubleInterop cuboidDoubleInterop = new CuboidDoubleInterop()
                            { X = cuboid.X, Y = cuboid.Y, Z = cuboid.Z, Width = cuboid.Width, Height = cuboid.Height, Depth = cuboid.Depth };
                            returnCode = this.idocread3dGetBricksIntersectingCuboid(
                                read3dHandle,
                                &cuboidDoubleInterop,
                                IntPtr.Zero,
                                new IntPtr(pointerTileInfoQueryClause),
                                new IntPtr(pointerQueryResultInterop),
                                &errorInformation);
                        }
                    }
                    else
                    {
                        // if (dimensionQueryClauseInterop != null && tileInfoQueryClauseInterop != null)
                        fixed (byte* pointerDimensionQueryClauseInterop = &dimensionQueryClauseInterop[0])
                        fixed (byte* pointerTileInfoQueryClause = &tileInfoQueryClauseInterop[0])
                        {
                            CuboidDoubleInterop cuboidDoubleInterop = new CuboidDoubleInterop()
                            { X = cuboid.X, Y = cuboid.Y, Z = cuboid.Z, Width = cuboid.Width, Height = cuboid.Height, Depth = cuboid.Depth };
                            returnCode = this.idocread3dGetBricksIntersectingCuboid(
                                read3dHandle,
                                &cuboidDoubleInterop,
                                new IntPtr(pointerDimensionQueryClauseInterop),
                                new IntPtr(pointerTileInfoQueryClause),
                                new IntPtr(pointerQueryResultInterop),
                                &errorInformation);
                        }
                    }
                }
            }

            this.HandleErrorCases(returnCode, in errorInformation);

            QueryResult result = ConvertToQueryResult(queryResultInterop);
            return result;
        }

        public QueryResult Reader3dQueryBricksIntersectingPlane(
           IntPtr read3dHandle,
           PlaneHesse plane,
           IDimensionQueryClause dimensionQueryClause,
           ITileInfoQueryClause tileInfoQueryClause,
           int maxNumberOfResults)
        {
            this.ThrowIfNotInitialized();
            byte[] dimensionQueryClauseInterop = dimensionQueryClause != null
                ? ConvertToTileCoordinateInterop(dimensionQueryClause)
                : null;
            byte[] tileInfoQueryClauseInterop = (tileInfoQueryClause != null)
                ? ConvertToTileInfoQueryInterop(tileInfoQueryClause)
                : null;
            byte[] queryResultInterop = CreateQueryResultInterop(maxNumberOfResults);

            int returnCode;
            ImgDoc2ErrorInformation errorInformation;

            unsafe
            {
                fixed (byte* pointerQueryResultInterop = &queryResultInterop[0])
                {
                    // TODO(JBL): this code is quite lame, maybe there is a better way which prevents us from having to code all those cases...
                    if (dimensionQueryClauseInterop == null && tileInfoQueryClauseInterop == null)
                    {
                        PlaneNormalAndDistanceInterop planeNormalAndDistanceInterop = new PlaneNormalAndDistanceInterop()
                        { NormalX = plane.NormalX, NormalY = plane.NormalY, NormalZ = plane.NormalZ, Distance = plane.Distance };
                        returnCode = this.idocread3dGetBricksIntersectingPlane(
                            read3dHandle,
                            &planeNormalAndDistanceInterop,
                            IntPtr.Zero,
                            IntPtr.Zero,
                            new IntPtr(pointerQueryResultInterop),
                            &errorInformation);
                    }
                    else if (dimensionQueryClauseInterop != null && tileInfoQueryClauseInterop == null)
                    {
                        fixed (byte* pointerDimensionQueryClauseInterop = &dimensionQueryClauseInterop[0])
                        {
                            PlaneNormalAndDistanceInterop planeNormalAndDistanceInterop = new PlaneNormalAndDistanceInterop()
                            { NormalX = plane.NormalX, NormalY = plane.NormalY, NormalZ = plane.NormalZ, Distance = plane.Distance };
                            returnCode = this.idocread3dGetBricksIntersectingPlane(
                                read3dHandle,
                                &planeNormalAndDistanceInterop,
                                new IntPtr(pointerDimensionQueryClauseInterop),
                                IntPtr.Zero,
                                new IntPtr(pointerQueryResultInterop),
                                &errorInformation);
                        }
                    }
                    else if (dimensionQueryClauseInterop == null && tileInfoQueryClauseInterop != null)
                    {
                        fixed (byte* pointerTileInfoQueryClause = &tileInfoQueryClauseInterop[0])
                        {
                            PlaneNormalAndDistanceInterop planeNormalAndDistanceInterop = new PlaneNormalAndDistanceInterop()
                            { NormalX = plane.NormalX, NormalY = plane.NormalY, NormalZ = plane.NormalZ, Distance = plane.Distance };
                            returnCode = this.idocread3dGetBricksIntersectingPlane(
                                read3dHandle,
                                &planeNormalAndDistanceInterop,
                                IntPtr.Zero,
                                new IntPtr(pointerTileInfoQueryClause),
                                new IntPtr(pointerQueryResultInterop),
                                &errorInformation);
                        }
                    }
                    else
                    {
                        // if (dimensionQueryClauseInterop != null && tileInfoQueryClauseInterop != null)
                        fixed (byte* pointerDimensionQueryClauseInterop = &dimensionQueryClauseInterop[0])
                        fixed (byte* pointerTileInfoQueryClause = &tileInfoQueryClauseInterop[0])
                        {
                            PlaneNormalAndDistanceInterop planeNormalAndDistanceInterop = new PlaneNormalAndDistanceInterop()
                            { NormalX = plane.NormalX, NormalY = plane.NormalY, NormalZ = plane.NormalZ, Distance = plane.Distance };
                            returnCode = this.idocread3dGetBricksIntersectingPlane(
                                read3dHandle,
                                &planeNormalAndDistanceInterop,
                                new IntPtr(pointerDimensionQueryClauseInterop),
                                new IntPtr(pointerTileInfoQueryClause),
                                new IntPtr(pointerQueryResultInterop),
                                &errorInformation);
                        }
                    }
                }
            }

            this.HandleErrorCases(returnCode, in errorInformation);

            QueryResult result = ConvertToQueryResult(queryResultInterop);
            return result;
        }

        public byte[] Reader2dReadTileData(IntPtr read2dHandle, long pk)
        {
            this.ThrowIfNotInitialized();
            int returnCode;
            ImgDoc2ErrorInformation errorInformation;

            BlobOutputOnByteArray blobOutput = new BlobOutputOnByteArray();

            unsafe
            {
                GCHandle gcHandle = GCHandle.Alloc(blobOutput, GCHandleType.Normal);
                returnCode = this.idocread2dReadTileData(
                    read2dHandle,
                    pk,
                    GCHandle.ToIntPtr(gcHandle),
                    this.funcPtrBlobOutputSetSizeForwarder,
                    this.funcPtrBlobOutputSetDataForwarder,
                    &errorInformation);
                gcHandle.Free();
            }

            this.HandleErrorCases(returnCode, in errorInformation);

            return blobOutput.Buffer;
        }

        public byte[] Reader3dReadBrickData(IntPtr read3dHandle, long pk)
        {
            this.ThrowIfNotInitialized();
            int returnCode;
            ImgDoc2ErrorInformation errorInformation;

            BlobOutputOnByteArray blobOutput = new BlobOutputOnByteArray();

            unsafe
            {
                GCHandle gcHandle = GCHandle.Alloc(blobOutput, GCHandleType.Normal);
                returnCode = this.idocread3dReadBrickData(
                    read3dHandle,
                    pk,
                    GCHandle.ToIntPtr(gcHandle),
                    this.funcPtrBlobOutputSetSizeForwarder,
                    this.funcPtrBlobOutputSetDataForwarder,
                    &errorInformation);
                gcHandle.Free();
            }

            this.HandleErrorCases(returnCode, in errorInformation);

            return blobOutput.Buffer;
        }

        public void Reader2dReadTileInfo(
            IntPtr read2dHandle,
            long pk,
            bool fillTileCoordinate,
            bool fillLogicalPositionInfo,
            bool fillTileBlobInfo,
            out ITileCoordinate coordinate,
            out LogicalPosition logicalPosition,
            out TileBlobInfo tileBlobInfo)
        {
            this.ThrowIfNotInitialized();
            coordinate = null;
            logicalPosition = default(LogicalPosition);
            tileBlobInfo = default(TileBlobInfo);

            ImgDoc2ErrorInformation errorInformation;

            unsafe
            {
                if (fillTileCoordinate)
                {
                    const int maxNumberOfCoordinates = 40;

                    int sizeForTileCoordinateInterop = TileCoordinateInterop.CalculateSize(maxNumberOfCoordinates);
                    byte* data = stackalloc byte[sizeForTileCoordinateInterop];
                    TileCoordinateInterop* pointerToTileCoordinateInterop = (TileCoordinateInterop*)data;
                    pointerToTileCoordinateInterop->NumberOfElements = maxNumberOfCoordinates;
                    LogicalPositionInfoInterop logicalPositionInfoInterop = default(LogicalPositionInfoInterop);
                    TileBlobInfoInterop tileBlobInfoInterop = default(TileBlobInfoInterop);
                    int returnCode = this.idocread2ReadTileInfo(
                        read2dHandle,
                        pk,
                        new IntPtr(pointerToTileCoordinateInterop),
                        fillLogicalPositionInfo ? &logicalPositionInfoInterop : null,
                        fillTileBlobInfo ? &tileBlobInfoInterop : null,
                        &errorInformation);

                    this.HandleErrorCases(returnCode, in errorInformation);
                    coordinate = ConvertToTileCoordinate(pointerToTileCoordinateInterop);
                    if (fillLogicalPositionInfo)
                    {
                        logicalPosition = ConvertToLogicalPosition(in logicalPositionInfoInterop);
                    }

                    if (fillTileBlobInfo)
                    {
                        tileBlobInfo = ConvertToTileBlobInfo(in tileBlobInfoInterop);
                    }
                }
                else
                {
                    LogicalPositionInfoInterop logicalPositionInfoInterop = default(LogicalPositionInfoInterop);
                    TileBlobInfoInterop tileBlobInfoInterop = default(TileBlobInfoInterop);
                    int returnCode = this.idocread2ReadTileInfo(
                        read2dHandle,
                        pk,
                        IntPtr.Zero,
                        fillLogicalPositionInfo ? &logicalPositionInfoInterop : null,
                        fillTileBlobInfo ? &tileBlobInfoInterop : null,
                        &errorInformation);

                    this.HandleErrorCases(returnCode, in errorInformation);

                    if (fillLogicalPositionInfo)
                    {
                        logicalPosition = ConvertToLogicalPosition(in logicalPositionInfoInterop);
                    }

                    if (fillTileBlobInfo)
                    {
                        tileBlobInfo = ConvertToTileBlobInfo(in tileBlobInfoInterop);
                    }
                }
            }
        }

        public void Reader3dReadBrickInfo(
            IntPtr read2dHandle,
            long pk,
            bool fillTileCoordinate,
            bool fillLogicalPositionInfo,
            bool fillTileBlobInfo,
            out ITileCoordinate coordinate,
            out LogicalPosition3d logicalPosition3d,
            out BrickBlobInfo brickBlobInfo)
        {
            this.ThrowIfNotInitialized();
            coordinate = null;
            logicalPosition3d = default(LogicalPosition3d);
            brickBlobInfo = default(BrickBlobInfo);

            ImgDoc2ErrorInformation errorInformation;

            unsafe
            {
                if (fillTileCoordinate)
                {
                    const int maxNumberOfCoordinates = 40;

                    int sizeForTileCoordinateInterop = TileCoordinateInterop.CalculateSize(maxNumberOfCoordinates);
                    byte* data = stackalloc byte[sizeForTileCoordinateInterop];
                    TileCoordinateInterop* pointerToTileCoordinateInterop = (TileCoordinateInterop*)data;
                    pointerToTileCoordinateInterop->NumberOfElements = maxNumberOfCoordinates;
                    LogicalPositionInfo3DInterop logicalPositionInfo3DInterop = default(LogicalPositionInfo3DInterop);
                    BrickBlobInfoInterop brickBlobInfoInterop = default(BrickBlobInfoInterop);
                    int returnCode = this.idocread3dReadBrickInfo(
                        read2dHandle,
                        pk,
                        new IntPtr(pointerToTileCoordinateInterop),
                        fillLogicalPositionInfo ? &logicalPositionInfo3DInterop : null,
                        fillTileBlobInfo ? &brickBlobInfoInterop : null,
                        &errorInformation);

                    this.HandleErrorCases(returnCode, in errorInformation);
                    coordinate = ConvertToTileCoordinate(pointerToTileCoordinateInterop);
                    if (fillLogicalPositionInfo)
                    {
                        logicalPosition3d = ConvertToLogicalPosition3D(in logicalPositionInfo3DInterop);
                    }

                    if (fillTileBlobInfo)
                    {
                        brickBlobInfo = ConvertToBrickBlobInfo(in brickBlobInfoInterop);
                    }
                }
                else
                {
                    LogicalPositionInfo3DInterop logicalPositionInfo3DInterop = default(LogicalPositionInfo3DInterop);
                    BrickBlobInfoInterop brickBlobInfoInterop = default(BrickBlobInfoInterop);
                    int returnCode = this.idocread3dReadBrickInfo(
                        read2dHandle,
                        pk,
                        IntPtr.Zero,
                        fillLogicalPositionInfo ? &logicalPositionInfo3DInterop : null,
                        fillTileBlobInfo ? &brickBlobInfoInterop : null,
                        &errorInformation);

                    this.HandleErrorCases(returnCode, in errorInformation);

                    if (fillLogicalPositionInfo)
                    {
                        logicalPosition3d = ConvertToLogicalPosition3D(in logicalPositionInfo3DInterop);
                    }

                    if (fillTileBlobInfo)
                    {
                        brickBlobInfo = ConvertToBrickBlobInfo(in brickBlobInfoInterop);
                    }
                }
            }
        }

        /// <summary> 
        /// Get the tile-dimensions of the document. This is corresponding to the native method 'IDocInfo::GetTileDimensions'.
        /// </summary>
        /// <param name="read2dHandle"> The reader-2d-object.</param>
        /// <returns> An array with the dimensions used in the document.</returns>
        public Dimension[] DocInfo2dGetTileDimensions(IntPtr read2dHandle)
        {
            return this.InternalDocInfo3dGetTileDimensions(this.idocinfo2dGetTileDimensions, read2dHandle);
        }

        public Dimension[] DocInfo3dGetTileDimensions(IntPtr read3dHandle)
        {
            return this.InternalDocInfo3dGetTileDimensions(this.idocinfo3dGetTileDimensions, read3dHandle);
        }

        public Dictionary<Dimension, (int Minimum, int Maximum)> DocInfo2dGetMinMaxForTileDimensions(IntPtr read2dHandle, IEnumerable<Dimension> dimensions)
        {
            return this.InternalDocInfoGetMinMaxForTileDimensions(this.idocinfo2dGetMinMaxForTileDimensions, read2dHandle, dimensions);
        }

        public Dictionary<Dimension, (int Minimum, int Maximum)> DocInfo3dGetMinMaxForTileDimensions(IntPtr read3dHandle, IEnumerable<Dimension> dimensions)
        {
            return this.InternalDocInfoGetMinMaxForTileDimensions(this.idocinfo3dGetMinMaxForTileDimensions, read3dHandle, dimensions);
        }

        /// <summary>   
        /// Retrieve the minimum axis aligned bounding box (of all tiles in the document). 
        /// If the values cannot be retrieved (e.g. if the document is empty), the min-values
        /// will be set to double.MaxValue and the max-values to double.MinValue.
        /// </summary>
        /// <param name="read2dHandle"> The reader-2d-object.</param>
        /// <returns>   A tuple containing the respective intervals. </returns>
        public (double minX, double maxX, double minY, double maxY) DocInfo2dGetTilesBoundingBox(IntPtr read2dHandle)
        {
            this.ThrowIfNotInitialized();
            unsafe
            {
                ImgDoc2ErrorInformation errorInformation = default(ImgDoc2ErrorInformation);
                double minX = 0.0;
                double maxX = 0.0;
                double minY = 0.0;
                double maxY = 0.0;
                int returnCode = this.idocinfo2dGetBoundingBoxForTiles(read2dHandle, &minX, &maxX, &minY, &maxY, &errorInformation);
                this.HandleErrorCases(returnCode, errorInformation);
                return (minX, maxX, minY, maxY);
            }
        }

        /// <summary>   
        /// Retrieve the minimum axis aligned bounding box (of all bricks in the document). 
        /// If the values cannot be retrieved (e.g. if the document is empty), the min-values
        /// will be set to double.MaxValue and the max-values to double.MinValue.
        /// </summary>
        /// <param name="read3dHandle"> The reader-3d-object.</param>
        /// <returns>   A tuple containing the respective intervals. </returns>
        public (double minX, double maxX, double minY, double maxY, double minZ, double maxZ) DocInfo3dGetTilesBoundingBox(IntPtr read3dHandle)
        {
            this.ThrowIfNotInitialized();
            unsafe
            {
                ImgDoc2ErrorInformation errorInformation = default(ImgDoc2ErrorInformation);
                double minX = 0.0;
                double maxX = 0.0;
                double minY = 0.0;
                double maxY = 0.0;
                double minZ = 0.0;
                double maxZ = 0.0;
                int returnCode = this.idocinfo3dGetBoundingBoxForBricks(read3dHandle, &minX, &maxX, &minY, &maxY, &minZ, &maxZ, &errorInformation);
                this.HandleErrorCases(returnCode, errorInformation);
                return (minX, maxX, minY, maxY, minZ, maxZ);
            }
        }

        /// <summary> Gets total number of tiles in the document.</summary>
        /// <param name="read2dHandle"> The reader-2d-object.</param>
        /// <returns> The total number of tiles in the document.</returns>
        public long DocInfo2dGetTotalTileCount(IntPtr read2dHandle)
        {
            return this.InternalDocInfoGetTotalTileCount(this.idocinfo2dGetTotalTileCount, read2dHandle);
        }

        /// <summary> Gets total number of tiles in the document.</summary>
        /// <param name="read3dHandle"> The reader-3d-object.</param>
        /// <returns> The total number of tiles in the document.</returns>
        public long DocInfo3dGetTotalTileCount(IntPtr read3dHandle)
        {
            return this.InternalDocInfoGetTotalTileCount(this.idocinfo3dGetTotalTileCount, read3dHandle);
        }

        /// <summary> Gets the total number of tiles per pyramid layer.</summary>
        /// <param name="read2dHandle"> The reader-2d-object.</param>
        /// <returns> A dictionary, where key is the pyramid layer number, and value is the total number of tiles (on this layer) in the document. </returns>
        public Dictionary<int, long> DocInfo2dGetTileCountPerPyramidLayer(IntPtr read2dHandle)
        {
            return this.InternalDocInfoGetTileCountPerPyramidLayer(this.idocinfo2dGetTileCountPerLayer, read2dHandle);
        }

        /// <summary> Gets the total number of tiles per pyramid layer.</summary>
        /// <param name="read3dHandle"> The reader-3d-object.</param>
        /// <returns> A dictionary, where key is the pyramid layer number, and value is the total number of tiles (on this layer) in the document. </returns>
        public Dictionary<int, long> DocInfo3dGetTileCountPerPyramidLayer(IntPtr read3dHandle)
        {
            return this.InternalDocInfoGetTileCountPerPyramidLayer(this.idocinfo3dGetTileCountPerLayer, read3dHandle);
        }

        /// <summary> Helper method that handles the interop with functions which return a string (using a semantic like with 
        ///           "createOptionsGetFilename" or "openExistingOptionsGetFilename").</summary>
        /// <exception cref="Exception"> Thrown when an exception error condition occurs.</exception>
        /// <param name="getString">    Delegate which calls into the native code. The first argument is the "char*" argument, the second the "size*" argument
        ///                             (both as an IntPtr).</param>
        /// <param name="functionName"> Name of the function (use for constructing an appropriate exception text).</param>
        /// <returns> The resulting string.</returns>
        private string GetStringInteropHelper(Func<IntPtr, IntPtr, int> getString, string functionName)
        {
            this.ThrowIfNotInitialized();

            const int initialLength = 1024;

            // TODO(JBL): we are abusing UIntPtr as an equivalent to size_t, c.f. https://stackoverflow.com/questions/32906774/what-is-equal-to-the-c-size-t-in-c-sharp
            UIntPtr sizeOfBuffer = new UIntPtr(initialLength);
            byte[] buffer = new byte[sizeOfBuffer.ToUInt32()];
            int returnCode;
            unsafe
            {
                fixed (byte* pointerToBuffer = &buffer[0])
                {
                    returnCode = getString(new IntPtr(pointerToBuffer), new IntPtr(&sizeOfBuffer));
                }

                if (returnCode == ImgDoc2_ErrorCode_OK && sizeOfBuffer.ToUInt32() > initialLength)
                {
                    Array.Resize(ref buffer, (int)sizeOfBuffer.ToUInt32());
                    fixed (byte* pointerToBuffer = &buffer[0])
                    {
                        returnCode = getString(new IntPtr(pointerToBuffer), new IntPtr(&sizeOfBuffer));
                    }
                }
            }

            if (returnCode != ImgDoc2_ErrorCode_OK)
            {
                // TODO(Jbl) : stretch out error-handling
                throw new Exception("Error from 'CreateOptionsGetFilename'.");
            }

            var filename = Encoding.UTF8.GetString(buffer, 0, (int)(sizeOfBuffer.ToUInt32() - 1));
            return filename;
        }
    }

    internal partial class ImgDoc2ApiInterop
    {
        public (Memory<byte>, int) DecodeJpgXr(Span<byte> compressedData, PixelType pixelType, int width, int height, int stride = -1)
        {
            return this.Decode(compressedData, DataType.JpgXrCompressedBitmap, pixelType, width, height, stride);
        }

        public (Memory<byte>, int) DecodeZstd0(Span<byte> compressedData, PixelType pixelType, int width, int height, int stride = -1)
        {
            return this.Decode(compressedData, DataType.Zstd0CcompressedBitmap, pixelType, width, height, stride);
        }

        public (Memory<byte>, int) DecodeZstd1(Span<byte> compressedData, PixelType pixelType, int width, int height, int stride = -1)
        {
            return this.Decode(compressedData, DataType.Zstd1CcompressedBitmap, pixelType, width, height, stride);
        }

        public (Memory<byte>, int) Decode(Span<byte> compressedData, DataType dataType, PixelType pixelType, int width, int height, int stride = -1)
        {
            this.ThrowIfNotInitialized();

            unsafe
            {
                BitmapInfoInterop bitmapInfoInterop = default(BitmapInfoInterop);
                bitmapInfoInterop.PixelType = (byte)pixelType;
                bitmapInfoInterop.PixelWidth = (uint)width;
                bitmapInfoInterop.PixelHeight = (uint)height;

                DecodedImageResultInterop result = default(DecodedImageResultInterop);
                ImgDoc2ErrorInformation errorInformation;

                fixed (byte* pointerToCompressedData = compressedData)
                {
                    int returnCode = this.decodeImage(
                        &bitmapInfoInterop,
                        (byte)dataType,
                        new IntPtr(pointerToCompressedData),
                        (ulong)compressedData.Length,
                        stride <= 0 ? 0 : (uint)stride,
                        this.funcPtrAllocateMemoryByteArray,
                        &result,
                        &errorInformation);

                    if (returnCode != ImgDoc2_ErrorCode_OK)
                    {
                        throw new Exception("Error from 'DecodeImageJpgXr'.");
                    }
                }

                return (ImgDoc2ApiInterop.ConvertAllocationObjectToByteArrayAndFreeGcHandle(ref result.Bitmap), (int)result.Stride);
            }
        }
    }

    /// <content> 
    /// Here we gather "simple overloads" of the interface functions.
    /// </content>
    internal partial class ImgDoc2ApiInterop
    {
        public long Writer2dAddTile(IntPtr write2dHandle, ITileCoordinate coordinate, in LogicalPosition logicalPosition, Tile2dBaseInfo tile2dBaseInfo, DataType dataType, byte[] data)
        {
            if (data != null && data.Length > 0)
            {
                unsafe
                {
                    fixed (byte* pointer = &data[0])
                    {
                        return this.Writer2dAddTile(write2dHandle, coordinate, in logicalPosition, tile2dBaseInfo, dataType, new IntPtr(pointer), data.Length);
                    }
                }
            }
            else
            {
                return this.Writer2dAddTile(write2dHandle, coordinate, in logicalPosition, tile2dBaseInfo, dataType, IntPtr.Zero, 0);
            }
        }

        public long Writer3dAddBrick(IntPtr write3dHandle, ITileCoordinate coordinate, in LogicalPosition3d logicalPosition3d, Brick3dBaseInfo brick3dBaseInfo, DataType dataType, byte[] data)
        {
            if (data != null && data.Length > 0)
            {
                unsafe
                {
                    fixed (byte* pointer = &data[0])
                    {
                        return this.Writer3dAddBrick(write3dHandle, coordinate, in logicalPosition3d, brick3dBaseInfo, dataType, new IntPtr(pointer), data.Length);
                    }
                }
            }
            else
            {
                return this.Writer3dAddBrick(write3dHandle, coordinate, in logicalPosition3d, brick3dBaseInfo, dataType, IntPtr.Zero, 0);
            }
        }
    }

    /// <content> 
    /// The public class "QueryResult" is defined in this part.
    /// </content>
    internal partial class ImgDoc2ApiInterop
    {
        public class QueryResult
        {
            /// <summary>
            /// Initializes a new instance of the <see cref="QueryResult"/> class. Reserve the specified number of items.</summary>
            /// <param name="reservedSize"> Number of items to be reserved</param>
            public QueryResult(int reservedSize)
            {
                this.Keys = new List<long>(reservedSize);
            }

            /// <summary>
            /// Initializes a new instance of the <see cref="QueryResult"/> class. Constructor that prevents a default instance of this class from being created.
            /// </summary>
            private QueryResult()
            {
            }

            /// <summary> Gets or sets a value indicating whether the result is complete, i.e. if all the result fit in the allotted space.</summary>
            /// <value> True if the result complete; false otherwise.</value>
            public bool ResultComplete { get; set; }

            public List<long> Keys { get; }
        }
    }

    /// <content> 
    /// This part is concerned with "Blob-output"-implementation - which is a mechanism for returning binary blobs
    /// from the native code, where we allocation of the memory is to take place in the managed code.
    /// </content>
    internal partial class ImgDoc2ApiInterop
    {
        /// <summary>
        /// Delegate to the (static) BlobOutputSetSizeFunction-forwarder-function. It is important that this delegate does NOT get
        /// GCed (which is ensured in case of a static variable of course).
        /// </summary>
        private static readonly BlobOutputSetSizeDelegate BlobOutputSetSizeDelegateObj =
            ImgDoc2ApiInterop.BlobOutputSetSizeFunction;

        /// <summary>
        /// Delegate to the (static) BlobOutputSetDataFunction-forwarder-function. It is important that this delegate does NOT get
        /// GCed (which is ensured in case of a static variable of course).
        /// </summary>
        private static readonly BlobOutputSetDataDelegate BlobOutputSetDataDelegateObj =
            ImgDoc2ApiInterop.BlobOutputSetDataFunction;

        /// <summary>
        /// Delegate to the (static) AllocateMemoryFunctionByteArray-function. It is important that this delegate does NOT get
        /// GCed (which is ensured in case of a static variable of course).
        /// </summary>
        private static readonly AllocateMemoryDelegate AllocateMemoryByteArrayDelegateObj =
            ImgDoc2ApiInterop.AllocateMemoryFunctionByteArray;

        /// <summary>
        /// Function pointer (callable from unmanaged code) to the function "BlobOutputSetSizeFunction".
        /// </summary>
        private readonly IntPtr funcPtrBlobOutputSetSizeForwarder;

        /// <summary>
        /// Function pointer (callable from unmanaged code) to the function "BlobOutputSetDataFunction".
        /// </summary>
        private readonly IntPtr funcPtrBlobOutputSetDataForwarder;

        /// <summary>
        /// Function pointer (callable from unmanaged code) to the function "AllocateMemoryFunctionByteArray".
        /// This function is used for allocating memory as a byte-array.
        ///  </summary>
        private readonly IntPtr funcPtrAllocateMemoryByteArray;

        private delegate bool BlobOutputSetSizeDelegate(IntPtr blobOutputObjectHandle, ulong size);

        private delegate bool BlobOutputSetDataDelegate(IntPtr blobOutputObjectHandle, ulong offset, ulong size, IntPtr pointerToData);

        /// <summary>
        /// Definition of a delegate for the "AllocateMemoryFunction" function. The argument 'allocationObject' is a pointer 
        /// to the structure "AllocationObjectInterop".
        /// </summary>
        private delegate bool AllocateMemoryDelegate(ulong size, IntPtr allocationObject);

        /// <summary>   
        /// This interface is used for "returning data from the unmanaged code". The idea is that the
        /// actual memory allocation is done on the managed side.
        /// </summary>
        internal interface IBlobOutput
        {
            /// <summary> Sets the size (in bytes) the object can store. This must be called once, and before 'SetData' is called.</summary>
            /// <param name="size"> The size (in bytes).</param>
            /// <returns> True if it succeeds, false if it fails.</returns>
            bool SetSize(ulong size);

            /// <summary> 
            /// Copies the specified data into the internal buffer. The specified buffer must completely fit into the internal buffer.
            /// </summary>
            /// <exception cref="InvalidOperationException">Thrown if the object is not initialized.</exception>
            /// <param name="offset">        The offset (in the internal buffer) where the data is to be copied to.</param>
            /// <param name="size">          The size (in bytes).</param>
            /// <param name="pointerToData"> Pointer to the data.</param>
            /// <returns> True if it succeeds, false if it fails.</returns>
            bool SetData(ulong offset, ulong size, IntPtr pointerToData);
        }

        private static bool BlobOutputSetSizeFunction(IntPtr blobOutputObjectHandle, ulong size)
        {
            // TODO(Jbl) - add error-handling, we must not throw exceptions from here
            GCHandle gcHandle = GCHandle.FromIntPtr(blobOutputObjectHandle);
            IBlobOutput blobOutput = gcHandle.Target as IBlobOutput;
            return blobOutput.SetSize(size);
        }

        private static bool BlobOutputSetDataFunction(IntPtr blobOutputObjectHandle, ulong offset, ulong size, IntPtr pointerToData)
        {
            // TODO(Jbl) - add error-handling, we must not throw exceptions from here
            GCHandle gcHandle = GCHandle.FromIntPtr(blobOutputObjectHandle);
            IBlobOutput blobOutput = gcHandle.Target as IBlobOutput;
            return blobOutput.SetData(offset, size, pointerToData);
        }

        private static unsafe bool AllocateMemoryFunctionByteArray(ulong size, IntPtr allocationObject)
        {
            if (size > int.MaxValue)
            {
                // the sanity check failed - we cannot allocate memory with a size exceeding 'int.MaxValue'
                return false;
            }

            AllocationObjectInterop* allocationObjectPointer = (AllocationObjectInterop*)allocationObject.ToPointer();
            byte[] buffer = new byte[size];
            GCHandle gcHandle = GCHandle.Alloc(buffer, GCHandleType.Pinned);
            allocationObjectPointer->PointerToMemory = gcHandle.AddrOfPinnedObject();
            allocationObjectPointer->Handle = GCHandle.ToIntPtr(gcHandle);
            return true;
        }

        /// <summary>Simplistic implementation of the <see cref="IBlobOutput"/> interface. The data is stored in a .NET-byte array.</summary>
        internal class BlobOutputOnByteArray : IBlobOutput
        {
            private byte[] buffer;

            /// <summary> Gets the backing buffer.</summary>
            ///
            /// <value> The buffer.</value>
            public byte[] Buffer => this.buffer;

            /// <inheritdoc cref="IBlobOutput.SetSize"/>
            public bool SetSize(ulong size)
            {
                if (this.buffer != null)
                {
                    throw new InvalidOperationException("Object was already initialized.");
                }

                this.buffer = new byte[size];
                return true;
            }

            /// <inheritdoc cref="IBlobOutput.SetData"/>
            public bool SetData(ulong offset, ulong size, IntPtr pointerToData)
            {
                if (this.buffer == null)
                {
                    throw new InvalidOperationException("Object was not initialized.");
                }

                if (offset + size > (ulong)this.buffer.LongLength)
                {
                    throw new ArgumentException("The specified buffer exceeds the internal buffer.");
                }

                Marshal.Copy(pointerToData, this.buffer, (int)offset, (int)size);
                return true;
            }
        }
    }

    /// <content> 
    /// This part is concerned with constructing and implementing the "environment object".
    /// </content>
    internal partial class ImgDoc2ApiInterop
    {
        private static readonly EnvironmentCallbackFunctionLogDelegate EnvironmentCallbackFunctionLogDelegateObj =
            ImgDoc2ApiInterop.EnvironmentCallbackFunctionLog;

        private static readonly EnvironmentCallbackFunctionIsLevelActiveDelegate
            EnvironmentCallbackFunctionIsLevelActiveDelegateObj =
                ImgDoc2ApiInterop.EnvironmentCallbackFunctionIsLevelActive;

        private static readonly EnvironmentCallbackFunctionReportFatalErrorAndExitDelegate
            EnvironmentCallbackFunctionReportFatalErrorAndExitDelegateObj =
                ImgDoc2ApiInterop.EnvironmentCallbackFunctionReportFatalErrorAndExit;

        private IntPtr environmentObjectHandle;

        private IntPtr funcPtrEnvironmentLog;
        private IntPtr funcPtrEnvironmentIsLevelActive;
        private IntPtr funcPtrEnvironmentReportFatalErrorAndExit;

        private delegate void EnvironmentCallbackFunctionLogDelegate(IntPtr userParameter, int level, IntPtr messageUtf8);

        private delegate bool EnvironmentCallbackFunctionIsLevelActiveDelegate(IntPtr userParameter, int level);

        private delegate void EnvironmentCallbackFunctionReportFatalErrorAndExitDelegate(IntPtr userParameter, IntPtr messageUtf8);

        private static void EnvironmentCallbackFunctionLog(IntPtr userParameter, int level, IntPtr messageUtf8)
        {
        }

        private static bool EnvironmentCallbackFunctionIsLevelActive(IntPtr userParameter, int level)
        {
            return true;
        }

        private static void EnvironmentCallbackFunctionReportFatalErrorAndExit(IntPtr userParameter, IntPtr messageUtf8)
        {
        }

        private void InitializeEnvironmentObject()
        {
            this.funcPtrEnvironmentLog =
                Marshal.GetFunctionPointerForDelegate(ImgDoc2ApiInterop.EnvironmentCallbackFunctionLogDelegateObj);
            this.funcPtrEnvironmentIsLevelActive =
                Marshal.GetFunctionPointerForDelegate(ImgDoc2ApiInterop
                    .EnvironmentCallbackFunctionIsLevelActiveDelegateObj);
            this.funcPtrEnvironmentReportFatalErrorAndExit =
                Marshal.GetFunctionPointerForDelegate(ImgDoc2ApiInterop
                    .EnvironmentCallbackFunctionReportFatalErrorAndExitDelegateObj);

            this.environmentObjectHandle = this.createEnvironmentObject(
                IntPtr.Zero,
                this.funcPtrEnvironmentLog,
                this.funcPtrEnvironmentIsLevelActive,
                this.funcPtrEnvironmentReportFatalErrorAndExit);
        }
    }

    internal partial class ImgDoc2ApiInterop
    {
        /// <summary> Handles error cases - i.e. if the imgdoc2-API-return-code is indicating an error, we retrieve the
        ///           error-information from the interop-error-information struct and turn it into an appropriate
        ///           .NET exception.</summary>
        /// <param name="returnCode">       The imgdoc2-API-return code.</param>
        /// <param name="errorInformation"> Interop-error-information struct describing the error.</param>
        private void HandleErrorCases(int returnCode, in ImgDoc2ErrorInformation errorInformation)
        {
            if (returnCode != ImgDoc2_ErrorCode_OK)
            {
                string errorMessage;
                unsafe
                {
                    fixed (byte* messagePointerUtf8 = errorInformation.Message)
                    {
                        // we need to determine the length of the string (i.e. the position of the terminating '\0') in order
                        // to use 'Encoding.UTF8.GetString' for proper operation. .NET 7 seems to have 'Marshal.PtrToStringUtf8' which
                        // would do the job better.
                        int lengthOfUtf8String;
                        for (lengthOfUtf8String = 0;
                             lengthOfUtf8String < ImgDoc2ErrorInformationMessageMaxLength;
                             ++lengthOfUtf8String)
                        {
                            if (messagePointerUtf8[lengthOfUtf8String] == 0)
                            {
                                break;
                            }
                        }

                        errorMessage = Encoding.UTF8.GetString(messagePointerUtf8, lengthOfUtf8String);
                    }
                }

                throw new ImgDoc2Exception(returnCode, errorMessage);
            }
        }
    }

    /// <summary>   
    /// This part contains the declaration of the delegates and native structs. 
    /// </summary>
    internal partial class ImgDoc2ApiInterop
    {
        private const int ImgDoc2ErrorInformationMessageMaxLength = 200;

#pragma warning disable SA1310 // Field names should not contain underscore
        private const int ImgDoc2_ErrorCode_OK = 0;
        private const int ImgDoc2_ErrorCode_InvalidArgument = 1;
        private const int ImgDoc2_ErrorCode_UnspecifiedError = 50;
#pragma warning restore SA1310 // Field names should not contain underscore

        private readonly GetStatisticsDelegate getStatistics;
        private readonly VoidAndReturnIntPtrDelegate createCreateOptions;
        private readonly IntPtrAndErrorInformationReturnErrorCodeDelegate destroyCreateOptions;

        private readonly VoidAndReturnIntPtrDelegate createOpenExistingOptions;
        private readonly IntPtrAndErrorInformationReturnErrorCodeDelegate destroyOpenExistingOptions;

        private readonly CreateOptions_SetDocumentTypeDelegate createOptionsSetDocumentType;
        private readonly CreateOptions_GetDocumentTypeDelegate createOptionsGetDocumentType;
        private readonly CreateOptionsSetFilenameDelegate createOptionsSetFilename;
        private readonly CreateOptionsGetFilenameDelegate createOptionsGetFilename;
        private readonly CreateOptions_SetBooleanDelegate createOptionsSetUseSpatialIndex;
        private readonly CreateOptions_SetBooleanDelegate createOptionsSetUseBlobTable;
        private readonly CreateOptions_GetBooleanDelegate createOptionsGetUseSpatialIndex;
        private readonly CreateOptions_GetBooleanDelegate createOptionsGetUseBlobTable;
        private readonly CreateOptions_AddDimensionDelegate createOptionsAddDimension;
        private readonly CreateOptions_AddDimensionDelegate createOptionsAddIndexedDimension;
        private readonly CreateOptions_GetDimensionsDelegate createOptionsGetDimensions;
        private readonly CreateOptions_GetDimensionsDelegate createOptionsGetIndexedDimensions;

        private readonly OpenExistingOptionsSetFilenameDelegate openExistingOptionsSetFilename;
        private readonly OpenExistingOptionsGetFilenameDelegate openExistingOptionsGetFilename;

        private readonly CreateNewDocumentDelegate createNewDocument;
        private readonly OpenExistingDocumentDelegate openExistingDocument;
        private readonly IntPtrAndErrorInformationReturnErrorCodeDelegate destroyDocument;

        private readonly IDoc_GetObjectDelegate documentGetReader2d;
        private readonly IntPtrAndErrorInformationReturnErrorCodeDelegate destroyReader2d;
        private readonly IDoc_GetObjectDelegate documentGetWriter2d;
        private readonly IntPtrAndErrorInformationReturnErrorCodeDelegate destroyWriter2d;

        private readonly IDoc_GetObjectDelegate documentGetReader3d;
        private readonly IntPtrAndErrorInformationReturnErrorCodeDelegate destroyReader3d;
        private readonly IDoc_GetObjectDelegate documentGetWriter3d;
        private readonly IntPtrAndErrorInformationReturnErrorCodeDelegate destroyWriter3d;

        private readonly IDocWrite2d_AddTileDelegate idocwrite2dAddTile;
        private readonly IDocRead2d3d_QueryDelegate idocread2dQuery;
        private readonly IDocRead2d_GetTilesIntersectingRectDelegate idocread2dGetTilesIntersectingRect;
        private readonly IDocRead2d_ReadTileDataDelegate idocread2dReadTileData;
        private readonly IDocRead2d_ReadTileInfoDelegate idocread2ReadTileInfo;

        private readonly IDocWrite3d_AddBrickDelegate idocwrite3dAddBrick;
        private readonly IDocRead2d3d_QueryDelegate idocread3dQuery;
        private readonly IDocRead3d_GetBricksIntersectingCuboidDelegate idocread3dGetBricksIntersectingCuboid;
        private readonly IDocRead3d_GetBricksIntersectingPlaneDelegate idocread3dGetBricksIntersectingPlane;
        private readonly IDocRead3d_ReadBrickInfoDelegate idocread3dReadBrickInfo;
        private readonly IDocRead3d_ReadBrickDataDelegate idocread3dReadBrickData;

        private readonly CreateEnvironmentObjectDelegate createEnvironmentObject;

        private readonly IDocInfo_GetTileDimensionsDelegate idocinfo2dGetTileDimensions;
        private readonly IDocInfo_GetTileDimensionsDelegate idocinfo3dGetTileDimensions;
        private readonly IDocInfo_GetMinMaxForTileDimensionsDelegate idocinfo2dGetMinMaxForTileDimensions;
        private readonly IDocInfo_GetMinMaxForTileDimensionsDelegate idocinfo3dGetMinMaxForTileDimensions;
        private readonly IDocInfo2d_GetBoundingBoxForTilesDelegate idocinfo2dGetBoundingBoxForTiles;
        private readonly IDocInfo3d_GetBoundingBoxForBricksDelegate idocinfo3dGetBoundingBoxForBricks;
        private readonly IDocInfo_GetTotalTileCountDelegate idocinfo2dGetTotalTileCount;
        private readonly IDocInfo_GetTotalTileCountDelegate idocinfo3dGetTotalTileCount;
        private readonly IDocInfo_GetTileCountPerLayerDelegate idocinfo2dGetTileCountPerLayer;
        private readonly IDocInfo_GetTileCountPerLayerDelegate idocinfo3dGetTileCountPerLayer;

        private readonly IDocWrite2d3d_BeginCommitRollbackTransactionDelegate idocwrite2dBeginTransaction;
        private readonly IDocWrite2d3d_BeginCommitRollbackTransactionDelegate idocwrite2dCommitTransaction;
        private readonly IDocWrite2d3d_BeginCommitRollbackTransactionDelegate idocwrite2dRollbackTransaction;
        private readonly IDocWrite2d3d_BeginCommitRollbackTransactionDelegate idocwrite3dBeginTransaction;
        private readonly IDocWrite2d3d_BeginCommitRollbackTransactionDelegate idocwrite3dCommitTransaction;
        private readonly IDocWrite2d3d_BeginCommitRollbackTransactionDelegate idocwrite3dRollbackTransaction;

        private readonly GetVersionInfoDelegate getVersionInfo;

        private readonly DecodeImageDelegate decodeImage;

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate void GetStatisticsDelegate(ImgDoc2StatisticsInterop* statisticsInterop);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate IntPtr VoidAndReturnIntPtrDelegate();

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int IntPtrAndErrorInformationReturnErrorCodeDelegate(IntPtr handle, ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int CreateOptions_SetDocumentTypeDelegate(IntPtr handle, byte documentType, ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int CreateOptions_GetDocumentTypeDelegate(IntPtr handle, byte* documentType, ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int CreateOptionsSetFilenameDelegate(IntPtr handle, IntPtr fileNameUtf8, ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int CreateOptionsGetFilenameDelegate(IntPtr handle, IntPtr fileNameUtf8, IntPtr size, ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int CreateOptions_SetBooleanDelegate(IntPtr handle, bool useSpatialIndex, ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int CreateOptions_GetBooleanDelegate(IntPtr handle, bool* useSpatialIndex, ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int CreateOptions_AddDimensionDelegate(IntPtr handle, byte dim, ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int CreateOptions_GetDimensionsDelegate(IntPtr handle, byte* dim, IntPtr dimElementCount, ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int OpenExistingOptionsSetFilenameDelegate(IntPtr handle, IntPtr fileNameUtf8, ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int OpenExistingOptionsGetFilenameDelegate(
            IntPtr handle,
            IntPtr fileNameUtf8,
            IntPtr size,
            ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int CreateNewDocumentDelegate(
            IntPtr handleCreateOptions,
            IntPtr handleEnvironmentObject,
            IntPtr* documentHandle,
            ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int OpenExistingDocumentDelegate(
            IntPtr openExistingOptions,
            IntPtr handleEnvironmentObject,
            IntPtr* documentHandle,
            ImgDoc2ErrorInformation* errorInformation);

        /// <summary>   Delegate used for the GetReader/GetWriter-methods of IDocument. </summary>
        /// <param name="documentHandle" type="IntPtr">                     Handle of the document. </param>
        /// <param name="reader2dHandle" type="IntPtr*">                    [out] If non-null, handle of the reader-2D-object will be put here. </param>
        /// <param name="errorInformation" type="ImgDoc2ErrorInformation*"> [in,out] If non-null, information describing an error. </param>
        /// <returns type="int">    An integer indicating success of failure of the operation. </returns>
        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int IDoc_GetObjectDelegate(IntPtr documentHandle, IntPtr* reader2dHandle, ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int IDocWrite2d_AddTileDelegate(
            IntPtr handle,
            IntPtr tileCoordinateInterop,
            LogicalPositionInfoInterop* logicalPositionInfoInterop,
            TileBaseInfoInterop* tileBaseInfo,
            byte dataType,
            IntPtr dataPtr,
            long size,
            long* resultPk,
            ImgDoc2ErrorInformation* errorInformation);

        /// <summary>   
        /// Delegate definition for IDocRead2d_Query/IDocRead3d_Query. This delegate is used for both 2d- and
        /// 3d-case since the signatures is identical.
        /// </summary>
        /// <param name="read2dHandle">                 Handle of the document (may be a read2d-handle in case of 2d-document, or a read3d-handle in case of 3d-document). </param>
        /// <param name="dimensionQueryClauseInterop">  The dimension query clause interop structure. </param>
        /// <param name="tileInfoQueryClauseInterop">   The tile information query clause interop structure. </param>
        /// <param name="queryResultInterop">           The query result interop  structure. </param>
        /// <param name="errorInformation">             [in,out] If non-null, in case of an error, additional information is put here.
        /// <returns>   An integer indicating error or success of the operation. </returns>
        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int IDocRead2d3d_QueryDelegate(
            IntPtr read2dHandle,
            IntPtr dimensionQueryClauseInterop,
            IntPtr tileInfoQueryClauseInterop,
            IntPtr queryResultInterop,
            ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int IDocRead2d_GetTilesIntersectingRectDelegate(
            IntPtr read2dHandle,
            RectangleDoubleInterop* rectangle,
            IntPtr dimensionQueryClauseInterop,
            IntPtr tileInfoQueryClauseInterop,
            IntPtr queryResultInterop,
            ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int IDocRead2d_ReadTileInfoDelegate(
            IntPtr read2dHandle,
            long pk,
            IntPtr tileCoordinateInterop,
            LogicalPositionInfoInterop* logicalPositionInfoInterop,
            TileBlobInfoInterop* tileBlobInfoInterop,
            ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int IDocRead2d_ReadTileDataDelegate(
            IntPtr read2dHandle,
            long pk,
            IntPtr blobOutputHandle,
            IntPtr functionPointerSetSize,
            IntPtr functionPointerSetData,
            ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int IDocWrite3d_AddBrickDelegate(
            IntPtr handle,
            IntPtr tileCoordinateInterop,
            LogicalPositionInfo3DInterop* logicalPositionInfoInterop,
            BrickBaseInfoInterop* tileBaseInfo,
            byte dataType,
            IntPtr dataPtr,
            long size,
            long* resultPk,
            ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int IDocRead3d_GetBricksIntersectingCuboidDelegate(
            IntPtr read3dHandle,
            CuboidDoubleInterop* cuboid,
            IntPtr dimensionQueryClauseInterop,
            IntPtr tileInfoQueryClauseInterop,
            IntPtr queryResultInterop,
            ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int IDocRead3d_GetBricksIntersectingPlaneDelegate(
            IntPtr read3dHandle,
            PlaneNormalAndDistanceInterop* planeNormalAndDistanceInterop,
            IntPtr dimensionQueryClauseInterop,
            IntPtr tileInfoQueryClauseInterop,
            IntPtr queryResultInterop,
            ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int IDocRead3d_ReadBrickInfoDelegate(
            IntPtr read3dHandle,
            long pk,
            IntPtr tileCoordinateInterop,
            LogicalPositionInfo3DInterop* logicalPositionInfoInterop,
            BrickBlobInfoInterop* tileBlobInfoInterop,
            ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int IDocRead3d_ReadBrickDataDelegate(
            IntPtr read3dHandle,
            long pk,
            IntPtr blobOutputHandle,
            IntPtr functionPointerSetSize,
            IntPtr functionPointerSetData,
            ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate IntPtr CreateEnvironmentObjectDelegate(
            IntPtr userParameter,
            IntPtr pfnLog,
            IntPtr pfnIsLevelActive,
            IntPtr reportFatalErrorAndExit);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int IDocInfo_GetTileDimensionsDelegate(
            IntPtr read2dHandle,
            IntPtr pointerToDimensionsArray,
            IntPtr pointerToCount,
            ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int IDocInfo_GetMinMaxForTileDimensionsDelegate(
            IntPtr read2dHandle,
            IntPtr pointerToDimensionsArray,
            uint pointerToCount,
            IntPtr pointerToMinMaxArray,
            ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int IDocInfo2d_GetBoundingBoxForTilesDelegate(
            IntPtr read2dHandle,
            double* minX,
            double* maxX,
            double* minY,
            double* maxY,
            ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int IDocInfo3d_GetBoundingBoxForBricksDelegate(
            IntPtr read3dHandle,
            double* minX,
            double* maxX,
            double* minY,
            double* maxY,
            double* minZ,
            double* maxZ,
            ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int IDocInfo_GetTotalTileCountDelegate(
            IntPtr read2dHandle,
            ulong* totalTileCount,
            ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int IDocInfo_GetTileCountPerLayerDelegate(
            IntPtr read2dHandle,
            TileCountPerLayerInterop* tileCountPerLayerInterop,
            ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int GetVersionInfoDelegate(
            VersionInfoInterop* versionInfoInterop,
            IntPtr allocMemoryFunctionPtr);

        /// <summary> 
        /// This delegate is corresponding the the APIs IDocWrite2d_BeginTransaction/IDocWrite2d_CommitTransaction/IDocWrite2d_RollbackTransaction
        /// and IDocWrite3d_BeginTransaction/IDocWrite3d_CommitTransaction/IDocWrite3d_RollbackTransaction.
        /// </summary>
        /// <param name="write2d3dHandle">  Handle of a writer2D or writer3D object.</param>
        /// <param name="errorInformation"> [in,out] If non-null and in case of an error, information describing the error is put here.</param>
        /// <returns> An error code.</returns>
        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int IDocWrite2d3d_BeginCommitRollbackTransactionDelegate(
            IntPtr write2d3dHandle,
            ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int DecodeImageDelegate(
            BitmapInfoInterop* bitmapInfoInterop,
            byte dataType,
            IntPtr pointerToCompressedData,
            ulong sizeOfCompressedData,
            uint destinationStride,
            IntPtr allocMemoryFunctionPtr,
            DecodedImageResultInterop* result,
            ImgDoc2ErrorInformation* errorInformation);

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        private unsafe struct ImgDoc2ErrorInformation
        {
            public fixed byte Message[ImgDoc2ErrorInformationMessageMaxLength];
        }

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        private struct DimensionAndValueInterop
        {
            public byte Dimension;
            public int Value;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        private struct AllocationObjectInterop
        {
            public IntPtr PointerToMemory;
            public IntPtr Handle;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        private struct VersionInfoInterop
        {
            public int Major;
            public int Minor;
            public int Patch;
            public AllocationObjectInterop CompilerIdentification;
            public AllocationObjectInterop BuildType;
            public AllocationObjectInterop RepositoryUrl;
            public AllocationObjectInterop RepositoryBranch;
            public AllocationObjectInterop RepositoryTag;
        }

        /// <summary>   
        /// This struct is used for transferring "tile-coordinate-information". The actual memory layout is that
        /// there is an array of "DimensionAndValueInterop" structs (so, no only one as in the definition below).
        /// There are as many "DimensionAndValueInterop"-structs in memory (contiguous) as the property "number_of_elements"
        /// is specifying.
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        private unsafe struct TileCoordinateInterop
        {
            public int NumberOfElements;

            /// <summary>   
            /// Here we have as many "DimensionAndValueInterop" structs directly following as "number_of_elements" is specifying.
            /// </summary>
            public DimensionAndValueInterop Values;

            public static int CalculateSize(int numberOfElements)
            {
                return (numberOfElements * Marshal.SizeOf<DimensionAndValueInterop>()) +
                       Marshal.SizeOf<TileCoordinateInterop>();
            }
        }

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        private unsafe struct LogicalPositionInfoInterop
        {
            public double PositionX;
            public double PositionY;
            public double Width;
            public double Height;
            public int PyramidLevel;

            public LogicalPositionInfoInterop(in LogicalPosition logicalPosition)
            {
                this.PositionX = logicalPosition.PositionX;
                this.PositionY = logicalPosition.PositionY;
                this.Width = logicalPosition.Width;
                this.Height = logicalPosition.Height;
                this.PyramidLevel = logicalPosition.PyramidLevel;
            }
        }

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        private struct TileBaseInfoInterop
        {
            public uint PixelWidth;
            public uint PixelHeight;
            public byte PixelType;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        private struct TileBlobInfoInterop
        {
            public TileBaseInfoInterop TileBaseInfo;

            public byte DataType;
        }

        /// <summary> 
        /// Statistics about the number of active object, provided by the interop-layer. This is
        /// intended to be used for debugging/testing purposes.
        /// This struct is corresponding to the unmanaged struct 'ImgDoc2StatisticsInterop'.
        /// </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        private struct ImgDoc2StatisticsInterop
        {
            public uint NumberOfCreateOptionsObjectsActive;
            public uint NumberOfOpenExistingOptionsObjectsActive;
            public uint NumberOfDocumentObjectsActive;
            public uint NumberOfReader2dObjectsActive;
            public uint NumberOfWriter2dObjectsActive;
            public uint NumberOfReader3dObjectsActive;
            public uint NumberOfWriter3dObjectsActive;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        private struct RectangleDoubleInterop
        {
            public double X;
            public double Y;
            public double Width;
            public double Height;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        private struct MinMaxInterop
        {
            public int Minimum;
            public int Maximum;
        }

        /// <summary> This struct gathers a pyramid-layer index and a count of tiles. It is corresponding to the unmanaged struct 'PerLayerTileCountInterop'.</summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        private struct PerLayerTileCountInterop
        {
            public int LayerIndex;
            public ulong TileCount;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        private struct CuboidDoubleInterop
        {
            public double X;
            public double Y;
            public double Z;
            public double Width;
            public double Height;
            public double Depth;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        private struct PlaneNormalAndDistanceInterop
        {
            public double NormalX;
            public double NormalY;
            public double NormalZ;
            public double Distance;
        }

        /// <summary> This struct is corresponding to the unmanaged struct 'LogicalPositionInfo3DInterop'. </summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        private struct LogicalPositionInfo3DInterop
        {
            public double PositionX;
            public double PositionY;
            public double PositionZ;
            public double Width;
            public double Height;
            public double Depth;
            public int PyramidLevel;

            public LogicalPositionInfo3DInterop(in LogicalPosition3d logicalPosition3d)
            {
                this.PositionX = logicalPosition3d.PositionX;
                this.PositionY = logicalPosition3d.PositionY;
                this.PositionZ = logicalPosition3d.PositionZ;
                this.Width = logicalPosition3d.Width;
                this.Height = logicalPosition3d.Height;
                this.Depth = logicalPosition3d.Depth;
                this.PyramidLevel = logicalPosition3d.PyramidLevel;
            }
        }

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        private struct BrickBaseInfoInterop
        {
            /// <summary> Width of the brick in unit of pixels.</summary>
            public uint PixelWidth;

            /// <summary> Height of the brick in unit of pixels.</summary>
            public uint PixelHeight;

            /// <summary> Depth of the brick in unit of pixels.</summary>
            public uint PixelDepth;

            public byte PixelType;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        private struct BrickBlobInfoInterop
        {
            public BrickBaseInfoInterop BrickBaseInfo;
            public byte DataType;
        }

        /// <summary> This struct is used for the 'IDocInfo_GetTileCountPerLayer'-API. It corresponds to the unmanaged struct 'TileCountPerLayerInterop'.</summary>
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        private struct TileCountPerLayerInterop
        {
            /// <summary> 
            /// The number of elements in the array 'pyramid_layer_and_tile_count' - i.e. the number of elements
            ///  for which space is allocated.
            /// </summary>
            public uint ElementCountAllocated;

            /// <summary> 
            /// On input, this number is not used. On output, it contains the number of available results.
            /// This number may be larger than 'element_count_allocated', and if this is the case, it
            /// indicates that not all results could be returned.
            /// The number of valid items in 'pyramid_layer_and_tile_count' in any case is the minimum
            /// of 'element_count_allocated' and 'element_count_available'.
            /// </summary>
            public uint ElementCountAvailable;

            /// <summary> This is actually an array of PerLayerTileCountInterop-struct (or - it is used as if it were an array).</summary>
            public PerLayerTileCountInterop PyramidLayerAndTileCount;

            /// <summary> Calculates the size of this structure large enough to hold the specified number of elements.</summary>
            /// <param name="numberOfElements"> Number of elements.</param>
            /// <returns> The calculated size in bytes.</returns>
            public static int CalculateSize(int numberOfElements)
            {
                return (numberOfElements * Marshal.SizeOf<PerLayerTileCountInterop>()) + (2 * Marshal.SizeOf<uint>());
            }
        }

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        private struct DecodedImageResultInterop
        {
            public uint Stride;
            public AllocationObjectInterop Bitmap;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        private struct BitmapInfoInterop
        {
            public byte PixelType;
            public uint PixelWidth;
            public uint PixelHeight;
        }
    }

    /// <summary>   
    /// This part contains internally used utilities and helper functions.
    /// </summary>
    internal partial class ImgDoc2ApiInterop
    {
        /// <summary>   
        /// Converts a tileCoordinate to a blittable data-structure.
        /// </summary>
        /// <remarks>
        /// A blittable data-structure or data type has an identical representation in managed and unmanaged code. So, it can be used in
        /// interop scenarios without further ado.
        /// </remarks>
        /// <param name="tileCoordinate" type="ITileCoordinate">    The tile coordinate. </param>
        /// <returns type="byte[]"> A byte array containing the tile coordinate information as a flat and blittable data-structure (following the "TileCoordinateInterop" struct layout). </returns>
        private static byte[] ConvertToTileCoordinateInterop(ITileCoordinate tileCoordinate)
        {
            int numberOfElements = tileCoordinate.EnumCoordinates().Count();
            if (numberOfElements == 0)
            {
                using (var stream = new MemoryStream(4))
                {
                    using (var writer = new BinaryWriter(stream))
                    {
                        writer.Write(0);
                    }

                    return stream.ToArray();
                }
            }

            // calculate the required size
            int size = TileCoordinateInterop.CalculateSize(numberOfElements);

            using (var stream = new MemoryStream(size))
            {
                using (var writer = new BinaryWriter(stream))
                {
                    writer.Write(numberOfElements);
                    foreach (var item in tileCoordinate.EnumCoordinates())
                    {
                        writer.Write((byte)item.Item1.Id);
                        writer.Write((byte)0);
                        writer.Write((byte)0);
                        writer.Write((byte)0);
                        writer.Write(item.Item2);
                    }
                }

                return stream.ToArray();
            }
        }

        private static byte[] ConvertToTileCoordinateInterop(IDimensionQueryClause dimensionQueryClause)
        {
            var conditions = dimensionQueryClause.EnumConditions().ToList();
            using (var stream = new MemoryStream())
            {
                using (var writer = new BinaryWriter(stream))
                {
                    writer.Write(conditions.Count);

                    foreach (var condition in conditions)
                    {
                        writer.Write((byte)condition.Dimension.Id);
                        writer.Write(condition.RangeStart);
                        writer.Write(condition.RangeEnd);
                    }
                }

                return stream.ToArray();
            }
        }

        private static byte[] ConvertToTileInfoQueryInterop(ITileInfoQueryClause tileInfoQueryClause)
        {
            int numberOfPyramidLevelConditions = tileInfoQueryClause.PyramidLevelConditions.Count();
            using (var stream = new MemoryStream())
            {
                using (var writer = new BinaryWriter(stream))
                {
                    writer.Write(numberOfPyramidLevelConditions);
                    foreach (var queryClause in tileInfoQueryClause.PyramidLevelConditions)
                    {
                        writer.Write((byte)queryClause.LogicalOperator);
                        writer.Write((byte)queryClause.ComparisonOperator);
                        writer.Write((ushort)0); // for alignment reasons
                        writer.Write(queryClause.Value);
                    }
                }

                return stream.ToArray();
            }
        }

        /// <summary>   
        /// Creates a blob containing an empty "query-result" with space for the specified amount of elements.
        /// This is corresponding to the native struct "QueryResultInterop".
        /// </summary>
        /// <param name="elementCount" type="int">  Reserve space for the specified number of elements. </param>
        /// <returns type="byte[]"> A blob representing the native structure "QueryResultInterop". </returns>
        private static byte[] CreateQueryResultInterop(int elementCount)
        {
            /*
             The native struct is:
            struct QueryResultInterop
            {
                std::uint32_t element_count;
                std::uint32_t more_results_available;
                imgdoc2::dbIndex indices[];
            };
            */

            int size = 8 + (8 * elementCount);
            using (var stream = new MemoryStream(new byte[size]))
            {
                using (var writer = new BinaryWriter(stream))
                {
                    writer.Write(elementCount);
                }

                return stream.ToArray();
            }
        }

        private static QueryResult ConvertToQueryResult(byte[] queryResultInterop)
        {
            int count = BitConverter.ToInt32(queryResultInterop, 0);
            QueryResult queryResult = new QueryResult(count)
            {
                ResultComplete = BitConverter.ToInt32(queryResultInterop, 4) == 0,
            };

            for (int i = 0; i < count; ++i)
            {
                queryResult.Keys.Add(BitConverter.ToInt64(queryResultInterop, 8 + (i * 8)));
            }

            return queryResult;
        }

        private static TileBaseInfoInterop ConvertToTileBaseInfoInterop(Tile2dBaseInfo tile2dBaseInfo)
        {
            var tileBaseInfoInterop = default(TileBaseInfoInterop);
            tileBaseInfoInterop.PixelWidth = (uint)tile2dBaseInfo.PixelWidth;
            tileBaseInfoInterop.PixelHeight = (uint)tile2dBaseInfo.PixelHeight;
            tileBaseInfoInterop.PixelType = (byte)tile2dBaseInfo.PixelType;
            return tileBaseInfoInterop;
        }

        private static BrickBaseInfoInterop ConvertToBrickBaseInfoInterop(Brick3dBaseInfo brick3dBaseInfo)
        {
            var brickBaseInfoInterop = default(BrickBaseInfoInterop);
            brickBaseInfoInterop.PixelWidth = (uint)brick3dBaseInfo.PixelWidth;
            brickBaseInfoInterop.PixelHeight = (uint)brick3dBaseInfo.PixelHeight;
            brickBaseInfoInterop.PixelDepth = (uint)brick3dBaseInfo.PixelDepth;
            brickBaseInfoInterop.PixelType = (byte)brick3dBaseInfo.PixelType;
            return brickBaseInfoInterop;
        }

        private static LogicalPosition ConvertToLogicalPosition(
            in LogicalPositionInfoInterop logicalPositionInfoInterop)
        {
            LogicalPosition logicalPosition = default(LogicalPosition);
            logicalPosition.PositionX = logicalPositionInfoInterop.PositionX;
            logicalPosition.PositionY = logicalPositionInfoInterop.PositionY;
            logicalPosition.Width = logicalPositionInfoInterop.Width;
            logicalPosition.Height = logicalPositionInfoInterop.Height;
            logicalPosition.PyramidLevel = logicalPositionInfoInterop.PyramidLevel;
            return logicalPosition;
        }

        private static LogicalPosition3d ConvertToLogicalPosition3D(
            in LogicalPositionInfo3DInterop logicalPositionInfo3DInterop)
        {
            LogicalPosition3d logicalPosition3d = default(LogicalPosition3d);
            logicalPosition3d.PositionX = logicalPositionInfo3DInterop.PositionX;
            logicalPosition3d.PositionY = logicalPositionInfo3DInterop.PositionY;
            logicalPosition3d.PositionZ = logicalPositionInfo3DInterop.PositionZ;
            logicalPosition3d.Width = logicalPositionInfo3DInterop.Width;
            logicalPosition3d.Height = logicalPositionInfo3DInterop.Height;
            logicalPosition3d.Depth = logicalPositionInfo3DInterop.Depth;
            logicalPosition3d.PyramidLevel = logicalPositionInfo3DInterop.PyramidLevel;
            return logicalPosition3d;
        }

        private static Tile2dBaseInfo ConvertToTile2dBaseInfo(in TileBaseInfoInterop tileBaseInfoInterop)
        {
            Tile2dBaseInfo tile2dBaseInfo = new Tile2dBaseInfo(
                (int)tileBaseInfoInterop.PixelWidth,
                (int)tileBaseInfoInterop.PixelHeight,
                (PixelType)tileBaseInfoInterop.PixelType); // TODO: check conversion to int and enum
            return tile2dBaseInfo;
        }

        private static Brick3dBaseInfo ConvertToBrick3dBaseInfo(in BrickBaseInfoInterop brickBaseInfoInterop)
        {
            Brick3dBaseInfo brick3dBaseInfo = new Brick3dBaseInfo(
                (int)brickBaseInfoInterop.PixelWidth,
                (int)brickBaseInfoInterop.PixelHeight,
                (int)brickBaseInfoInterop.PixelDepth,
                (PixelType)brickBaseInfoInterop.PixelType); // TODO: check conversion to int and enum
            return brick3dBaseInfo;
        }

        private static TileBlobInfo ConvertToTileBlobInfo(in TileBlobInfoInterop tileBlobInfoInterop)
        {
            TileBlobInfo tileBlobInfo = new TileBlobInfo
            {
                Tile2dBaseInfo = ConvertToTile2dBaseInfo(in tileBlobInfoInterop.TileBaseInfo),
                DataType = (DataType)tileBlobInfoInterop.DataType, // TODO: check enum
            };

            return tileBlobInfo;
        }

        private static BrickBlobInfo ConvertToBrickBlobInfo(in BrickBlobInfoInterop brickBlobInfoInterop)
        {
            BrickBlobInfo brickBlobInfo = new BrickBlobInfo
            {
                Brick3dBaseInfo = ConvertToBrick3dBaseInfo(in brickBlobInfoInterop.BrickBaseInfo),
                DataType = (DataType)brickBlobInfoInterop.DataType, // TODO: check enum
            };

            return brickBlobInfo;
        }

        private static unsafe TileCoordinate ConvertToTileCoordinate(TileCoordinateInterop* tileCoordinateInterop)
        {
            Tuple<Dimension, int>[] dimensionValueTuples =
                new Tuple<Dimension, int>[tileCoordinateInterop->NumberOfElements];
            DimensionAndValueInterop* pointerDimensionAndValueInterop = &tileCoordinateInterop->Values;
            for (int i = 0; i < tileCoordinateInterop->NumberOfElements; ++i)
            {
                dimensionValueTuples[i] = Tuple.Create(
                    new Dimension((char)pointerDimensionAndValueInterop->Dimension),
                    pointerDimensionAndValueInterop->Value);
                ++pointerDimensionAndValueInterop;
            }

            TileCoordinate tileCoordinate = new TileCoordinate(dimensionValueTuples);
            return tileCoordinate;
        }

        private static void FreeAllocationObject(ref AllocationObjectInterop allocationObjectInterop)
        {
            if (allocationObjectInterop.PointerToMemory != IntPtr.Zero)
            {
                GCHandle gcHandle = GCHandle.FromIntPtr(allocationObjectInterop.Handle);
                gcHandle.Free();
                allocationObjectInterop.PointerToMemory = IntPtr.Zero;
            }
        }

        private static string ConvertAllocationObjectToString(in AllocationObjectInterop allocationObjectInterop)
        {
            if (allocationObjectInterop.PointerToMemory == IntPtr.Zero)
            {
                return string.Empty;
            }

            return Utilities.ConvertFromUtf8IntPtrZeroTerminated(allocationObjectInterop.PointerToMemory);
        }

        private static byte[] ConvertAllocationObjectToByteArrayAndFreeGcHandle(ref AllocationObjectInterop allocationObjectInterop)
        {
            if (allocationObjectInterop.PointerToMemory == IntPtr.Zero)
            {
                return Array.Empty<byte>();
            }

            GCHandle gcHandle = GCHandle.FromIntPtr(allocationObjectInterop.Handle);
            byte[] result = (byte[])gcHandle.Target;
            gcHandle.Free();
            allocationObjectInterop.PointerToMemory = IntPtr.Zero;
            return result;
        }

        private QueryResult InternalReaderQuery(IDocRead2d3d_QueryDelegate nativeQueryFunction, IntPtr handle, IDimensionQueryClause clause, ITileInfoQueryClause tileInfoQueryClause, int maxNumberOfResults)
        {
            this.ThrowIfNotInitialized();
            byte[] dimensionQueryClauseInterop = (clause != null) ? ConvertToTileCoordinateInterop(clause) : null;
            byte[] tileInfoQueryClauseInterop = (tileInfoQueryClause != null)
                ? ConvertToTileInfoQueryInterop(tileInfoQueryClause)
                : null;
            byte[] queryResultInterop = CreateQueryResultInterop(maxNumberOfResults);

            int returnCode;
            ImgDoc2ErrorInformation errorInformation;

            unsafe
            {
                fixed (byte* pointerQueryResultInterop = &queryResultInterop[0])
                {
                    if (dimensionQueryClauseInterop != null && tileInfoQueryClauseInterop != null)
                    {
                        fixed (byte* pointerDimensionQueryClauseInterop = &dimensionQueryClauseInterop[0])
                        fixed (byte* pointerTileInfoQueryClauseInterop = &tileInfoQueryClauseInterop[0])
                        {
                            returnCode = nativeQueryFunction(
                                handle,
                                new IntPtr(pointerDimensionQueryClauseInterop),
                                new IntPtr(pointerTileInfoQueryClauseInterop),
                                new IntPtr(pointerQueryResultInterop),
                                &errorInformation);
                        }
                    }
                    else if (dimensionQueryClauseInterop != null && tileInfoQueryClauseInterop == null)
                    {
                        fixed (byte* pointerDimensionQueryClauseInterop = &dimensionQueryClauseInterop[0])
                        {
                            returnCode = nativeQueryFunction(
                                handle,
                                new IntPtr(pointerDimensionQueryClauseInterop),
                                IntPtr.Zero,
                                new IntPtr(pointerQueryResultInterop),
                                &errorInformation);
                        }
                    }
                    else if (dimensionQueryClauseInterop == null && tileInfoQueryClauseInterop != null)
                    {
                        fixed (byte* pointerTileInfoQueryClauseInterop = &tileInfoQueryClauseInterop[0])
                        {
                            returnCode = nativeQueryFunction(
                                handle,
                                IntPtr.Zero,
                                new IntPtr(pointerTileInfoQueryClauseInterop),
                                new IntPtr(pointerQueryResultInterop),
                                &errorInformation);
                        }
                    }
                    else
                    {
                        // if (dimensionQueryClauseInterop != null && tileInfoQueryClauseInterop != null)
                        returnCode = nativeQueryFunction(
                            handle,
                            IntPtr.Zero,
                            IntPtr.Zero,
                            new IntPtr(pointerQueryResultInterop),
                            &errorInformation);
                    }
                }
            }

            this.HandleErrorCases(returnCode, in errorInformation);

            QueryResult result = ConvertToQueryResult(queryResultInterop);
            return result;
        }

        private Dimension[] InternalDocInfo3dGetTileDimensions(IDocInfo_GetTileDimensionsDelegate nativeGetTileDimensionsDelegate, IntPtr handle)
        {
            this.ThrowIfNotInitialized();
            unsafe
            {
                const int initialArraySize = 20;    // number of elements for the initial buffer we supply
                ImgDoc2ErrorInformation errorInformation = default(ImgDoc2ErrorInformation);
                byte* dimensionsArray = stackalloc byte[initialArraySize];
                uint count = initialArraySize;
                int returnCode = nativeGetTileDimensionsDelegate(handle, new IntPtr(dimensionsArray), new IntPtr(&count), &errorInformation);
                this.HandleErrorCases(returnCode, errorInformation);
                if (count > initialArraySize)
                {
                    // if the buffer size was too small, we allocate a larger one (with the size reported) and try again
                    byte* dimensionsArray2 = stackalloc byte[(int)count];
                    returnCode = nativeGetTileDimensionsDelegate(handle, new IntPtr(dimensionsArray2), new IntPtr(&count), &errorInformation);
                    dimensionsArray = dimensionsArray2;
                }

                Dimension[] dimensions = new Dimension[count];
                for (int i = 0; i < count; ++i)
                {
                    dimensions[i] = new Dimension(Convert.ToChar(dimensionsArray[i]));
                }

                return dimensions;
            }
        }

        private Dictionary<Dimension, (int Minimum, int Maximum)> InternalDocInfoGetMinMaxForTileDimensions(IDocInfo_GetMinMaxForTileDimensionsDelegate nativeGetMinMaxForTileDimensionsDelegate, IntPtr handle, IEnumerable<Dimension> dimensions)
        {
            this.ThrowIfNotInitialized();
            int dimensionCount = dimensions.Count();
            unsafe
            {
                Span<byte> dimensionsArray = stackalloc byte[dimensionCount];
                int i = 0;
                foreach (var dimension in dimensions)
                {
                    dimensionsArray[i] = (byte)dimension.Id;
                    ++i;
                }

                MinMaxInterop[] minMaxInteropArray = new MinMaxInterop[dimensionCount];
                ImgDoc2ErrorInformation errorInformation = default(ImgDoc2ErrorInformation);

                fixed (byte* pointerToDimensionArray = dimensionsArray)
                fixed (MinMaxInterop* pointerToMinMaxArray = &minMaxInteropArray[0])
                {
                    int returnCode = nativeGetMinMaxForTileDimensionsDelegate(
                        handle,
                        new IntPtr(pointerToDimensionArray),
                        (uint)dimensionCount,
                        new IntPtr(pointerToMinMaxArray),
                        &errorInformation);
                }

                var result = new Dictionary<Dimension, (int Minimum, int Maximum)>(dimensionCount);

                i = 0;
                foreach (var d in dimensionsArray)
                {
                    result[new Dimension(Convert.ToChar(d))] = (minMaxInteropArray[i].Minimum, minMaxInteropArray[i].Maximum);
                    ++i;
                }

                return result;
            }
        }

        private long InternalDocInfoGetTotalTileCount(IDocInfo_GetTotalTileCountDelegate docInfoGetTotalTileCountDelegate, IntPtr handle)
        {
            this.ThrowIfNotInitialized();
            unsafe
            {
                ImgDoc2ErrorInformation errorInformation = default(ImgDoc2ErrorInformation);
                ulong totalTileCount = 0;
                int returnCode = docInfoGetTotalTileCountDelegate(handle, &totalTileCount, &errorInformation);
                this.HandleErrorCases(returnCode, errorInformation);
                return (long)totalTileCount;
            }
        }

        private Dictionary<int, long> InternalDocInfoGetTileCountPerPyramidLayer(IDocInfo_GetTileCountPerLayerDelegate docInfoGetTileCountPerLayerDelegate, IntPtr handle)
        {
            this.ThrowIfNotInitialized();
            unsafe
            {
                const int initialArraySize = 20;    // number of elements for the initial buffer we supply (which should be enough in most cases)
                byte* tileCountPerLayerInteropData = stackalloc byte[TileCountPerLayerInterop.CalculateSize(initialArraySize)];
                ImgDoc2ErrorInformation errorInformation = default(ImgDoc2ErrorInformation);
                TileCountPerLayerInterop* tileCountPerLayerInterop = (TileCountPerLayerInterop*)tileCountPerLayerInteropData;
                tileCountPerLayerInterop->ElementCountAllocated = initialArraySize;
                int returnCode = docInfoGetTileCountPerLayerDelegate(handle, tileCountPerLayerInterop, &errorInformation);
                this.HandleErrorCases(returnCode, errorInformation);
                if (tileCountPerLayerInterop->ElementCountAvailable > initialArraySize)
                {
                    // if the buffer size was too small, we allocate a larger one (with the size reported, this time on the heap) and try again
                    var elementCountRequired = (int)tileCountPerLayerInterop->ElementCountAvailable;
                    byte[] tileCountPerLayerInteropArray = new byte[TileCountPerLayerInterop.CalculateSize(elementCountRequired)];
                    fixed (byte* pointerTileCountPerLayerInteropArray = tileCountPerLayerInteropArray)
                    {
                        tileCountPerLayerInterop = (TileCountPerLayerInterop*)pointerTileCountPerLayerInteropArray;
                        tileCountPerLayerInterop->ElementCountAllocated = (uint)elementCountRequired;
                        returnCode = docInfoGetTileCountPerLayerDelegate(handle, tileCountPerLayerInterop, &errorInformation);
                        this.HandleErrorCases(returnCode, errorInformation);

                        // we do not expect that the size was insufficient in this case, and if so we throw an exception
                        if (tileCountPerLayerInterop->ElementCountAvailable > tileCountPerLayerInterop->ElementCountAllocated)
                        {
                            throw new InvalidOperationException("The buffer size was insufficient for the tile count per layer information, which is unexpected.");
                        }
                    }
                }

                Dictionary<int, long> result = new Dictionary<int, long>((int)tileCountPerLayerInterop->ElementCountAvailable);
                PerLayerTileCountInterop* perLayerTileCount = &tileCountPerLayerInterop->PyramidLayerAndTileCount;
                for (int i = 0; i < tileCountPerLayerInterop->ElementCountAvailable; ++i)
                {
                    result.Add(perLayerTileCount->LayerIndex, (long)perLayerTileCount->TileCount);
                    ++perLayerTileCount;
                }

                return result;
            }
        }

        private void InternalWriter2d3dTransaction(IntPtr handle2d3dWriter, IDocWrite2d3d_BeginCommitRollbackTransactionDelegate operation)
        {
            this.ThrowIfNotInitialized();
            int returnCode;
            ImgDoc2ErrorInformation errorInformation;
            unsafe
            {
                returnCode = operation(handle2d3dWriter, &errorInformation);
            }

            this.HandleErrorCases(returnCode, in errorInformation);
        }
    }
}