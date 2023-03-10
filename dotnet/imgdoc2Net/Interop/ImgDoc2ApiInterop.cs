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
    public partial class ImgDoc2ApiInterop
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
                    this.GetProcAddressThrowIfNotFound<IntPtrAndReturnVoidDelegate>("DestroyCreateOptions");
                this.createOpenExistingOptions =
                    this.GetProcAddressThrowIfNotFound<VoidAndReturnIntPtrDelegate>("CreateOpenExistingOptions");
                this.destroyOpenExistingOptions =
                    this.GetProcAddressThrowIfNotFound<IntPtrAndReturnVoidDelegate>("DestroyOpenExistingOptions");
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
                    this.GetProcAddressThrowIfNotFound<IntPtrAndReturnVoidDelegate>("DestroyDocument");

                this.documentGetReader2d =
                    this.GetProcAddressThrowIfNotFound<IDoc_GetObjectDelegate>("IDoc_GetReader2d");
                this.destroyReader2d =
                    this.GetProcAddressThrowIfNotFound<IntPtrAndReturnVoidDelegate>("DestroyReader2d");
                this.documentGetWriter2d =
                    this.GetProcAddressThrowIfNotFound<IDoc_GetObjectDelegate>("IDoc_GetWriter2d");
                this.destroyWriter2d =
                    this.GetProcAddressThrowIfNotFound<IntPtrAndReturnVoidDelegate>("DestroyWriter2d");

                this.idocwrite2dAddTile =
                    this.GetProcAddressThrowIfNotFound<IDocWrite2d_AddTileDelegate>("IDocWrite2d_AddTile");
                this.idocread2dQuery =
                    this.GetProcAddressThrowIfNotFound<IDocRead2d_QueryDelegate>("IDocRead2d_Query");
                this.idocread2dGetTilesIntersectingRect =
                    this.GetProcAddressThrowIfNotFound<IDocRead2d_GetTilesIntersectingRectDelegate>("IDocRead2d_GetTilesIntersectingRect");
                this.idocread2dReadTileData =
                    this.GetProcAddressThrowIfNotFound<IDocRead2d_ReadTileDataDelegate>("IDocRead2d_ReadTileData");
                this.idocread2ReadTileInfo =
                    this.GetProcAddressThrowIfNotFound<IDocRead2d_ReadTileInfoDelegate>("IDocRead2d_ReadTileInfo");

                this.funcPtrBlobOutputSetSizeForwarder =
                    Marshal.GetFunctionPointerForDelegate<BlobOutputSetSizeDelegate>(ImgDoc2ApiInterop.BlobOutputSetSizeDelegateObj);
                this.funcPtrBlobOutputSetDataForwarder =
                    Marshal.GetFunctionPointerForDelegate<BlobOutputSetDataDelegate>(ImgDoc2ApiInterop.BlobOutputSetDataDelegateObj);

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
            // TODO(JBL): I'd like to have CPU-architecture-specific suffixes for the filenames ("x86", "x64", "arm32" etc.) and probably a "d" for debug-builds or so
            string pathOfExecutable = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location) ?? string.Empty;
            bool isLinux = Utilities.IsLinux();

            if (!isLinux)
            {
                yield return Path.Combine(pathOfExecutable, BaseDllNameWindows + ".dll");
            }
            else
            {
                yield return Path.Combine(pathOfExecutable, BaseDllNameLinux + ".so");
            }
        }
    }

    /// <content>   
    /// In this part we find the wrappers for calling into native code. 
    /// </content>
    public partial class ImgDoc2ApiInterop
    {
        /// <summary>   Gets the "ImgDoc2Statistics". </summary>
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
            this.destroyCreateOptions(handleCreateOptions);
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

        /// <summary>   GSet the filename property of the specified "create-options" object.</summary>
        /// <param name="handleCreateOptions">  The handle of the "create-options" object to be queried. </param>
        /// <returns>   The filename property. </returns>
        public string CreateOptionsGetFilename(IntPtr handleCreateOptions)
        {
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

        public bool CreateOptionsGetUseSpatialIndex(IntPtr handleCreateOptions)
        {
            this.ThrowIfNotInitialized();
            bool useSpatialIndex;
            int returnCode;
            unsafe
            {
                returnCode = this.createOptionsGetUseSpatialIndex(handleCreateOptions, &useSpatialIndex, null);
            }

            if (returnCode != ImgDoc2_ErrorCode_OK)
            {
                // TODO(Jbl) : stretch out error-handling
                throw new Exception("Error from 'CreateOptionsGetUseSpatialIndex'.");
            }

            return useSpatialIndex;
        }

        public bool CreateOptionsGetUseBlobTable(IntPtr handleCreateOptions)
        {
            this.ThrowIfNotInitialized();
            bool useBlobTable;
            int returnCode;
            unsafe
            {
                returnCode = this.createOptionsGetUseBlobTable(handleCreateOptions, &useBlobTable, null);
            }

            if (returnCode != ImgDoc2_ErrorCode_OK)
            {
                // TODO(Jbl) : stretch out error-handling
                throw new Exception("Error from 'CreateOptionsGetUseBlobTable'.");
            }

            return useBlobTable;
        }

        public void CreateOptionsSetUseSpatialIndex(IntPtr handleCreateOptions, bool useSpatialIndex)
        {
            this.ThrowIfNotInitialized();
            int returnCode;
            unsafe
            {
                returnCode = this.createOptionsSetUseSpatialIndex(handleCreateOptions, useSpatialIndex, null);
            }

            if (returnCode != ImgDoc2_ErrorCode_OK)
            {
                // TODO(Jbl) : stretch out error-handling
                throw new Exception("Error from 'CreateOptionsGetFilename'.");
            }
        }

        public void CreateOptionsSetUseBlobTable(IntPtr handleCreateOptions, bool useBlobTable)
        {
            this.ThrowIfNotInitialized();
            int returnCode;
            unsafe
            {
                returnCode = this.createOptionsSetUseBlobTable(handleCreateOptions, useBlobTable, null);
            }

            if (returnCode != ImgDoc2_ErrorCode_OK)
            {
                // TODO(Jbl) : stretch out error-handling
                throw new Exception("Error from 'CreateOptionsGetFilename'.");
            }
        }

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

        public Dimension[] CreateOptionsGetDimensions(IntPtr handleCreateOptions)
        {
            this.ThrowIfNotInitialized();

            int returnCode;

            // for sake of simplicity, we only call once into native code with a buffer which is large enough for
            //  all conceivable cases
            const int BufferSize = 64;
            unsafe
            {
                byte* dimensionsBuffer = stackalloc byte[BufferSize];

                // TODO(Jbl):  we are abusing UIntPtr as an equivalent to size_t, c.f. https://stackoverflow.com/questions/32906774/what-is-equal-to-the-c-size-t-in-c-sharp
                UIntPtr sizeOfBuffer = new UIntPtr(BufferSize); 
                returnCode = this.createOptionsGetDimensions(handleCreateOptions, dimensionsBuffer, new IntPtr(&sizeOfBuffer), null);
                if (sizeOfBuffer.ToUInt32() > BufferSize)
                {
                    throw new NotImplementedException("Buffersize exceeded, may want to implement variable buffersizes.");
                }

                Dimension[] dimensions = new Dimension[sizeOfBuffer.ToUInt32()];
                for (int i = 0; i < sizeOfBuffer.ToUInt32(); i++)
                {
                    dimensions[i] = new Dimension((char)dimensionsBuffer[i]);
                }

                return dimensions;
            }
        }

        public Dimension[] CreateOptionsGetIndexedDimensions(IntPtr handleCreateOptions)
        {
            this.ThrowIfNotInitialized();

            int returnCode;

            // for sake of simplicity, we only call once into native code with a buffer which is large enough for
            //  all conceivable cases
            const int BufferSize = 64;
            unsafe
            {
                byte* dimensionsBuffer = stackalloc byte[BufferSize];
                UIntPtr
                    sizeOfBuffer =
                        new UIntPtr(
                            BufferSize); // TODO(Jbl):  we are abusing UIntPtr as an equivalent to size_t, c.f. https://stackoverflow.com/questions/32906774/what-is-equal-to-the-c-size-t-in-c-sharp
                returnCode = this.createOptionsGetIndexedDimensions(handleCreateOptions, dimensionsBuffer, new IntPtr(&sizeOfBuffer), null);
                if (sizeOfBuffer.ToUInt32() > BufferSize)
                {
                    throw new NotImplementedException(
                        "Buffersize exceeded, may want to implement variable buffersizes.");
                }

                Dimension[] dimensions = new Dimension[sizeOfBuffer.ToUInt32()];
                for (int i = 0; i < sizeOfBuffer.ToUInt32(); i++)
                {
                    dimensions[i] = new Dimension((char)dimensionsBuffer[i]);
                }

                return dimensions;
            }
        }

        public IntPtr CreateOpenExistingOptions()
        {
            this.ThrowIfNotInitialized();
            return this.createOpenExistingOptions();
        }

        public void DestroyOpenExistingOptions(IntPtr handle)
        {
            this.ThrowIfNotInitialized();
            this.destroyOpenExistingOptions(handle);
        }

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

        /// <summary>   Opens existing document. </summary>
        /// <param name="handleOpenExistingOptions">    Options for controlling the operation. </param>
        /// <returns>   A handle representing the document. </returns>
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

        public void DestroyDocument(IntPtr handleDocument)
        {
            this.ThrowIfNotInitialized();
            this.destroyDocument(handleDocument);
        }

        public IntPtr DocumentGetReader2d(IntPtr handleDocumnet)
        {
            this.ThrowIfNotInitialized();

            int returnCode;
            IntPtr reader2dHandle;
            ImgDoc2ErrorInformation errorInformation;
            unsafe
            {
                returnCode = this.documentGetReader2d(handleDocumnet, &reader2dHandle, &errorInformation);
            }

            this.HandleErrorCases(returnCode, in errorInformation);
            return reader2dHandle;
        }

        public void DestroyReader2d(IntPtr handleReader)
        {
            this.ThrowIfNotInitialized();
            this.destroyReader2d(handleReader);
        }

        public IntPtr DocumentGetWriter2d(IntPtr handleDocumnet)
        {
            this.ThrowIfNotInitialized();

            int returnCode;
            IntPtr writerHandle;
            ImgDoc2ErrorInformation errorInformation;
            unsafe
            {
                returnCode = this.documentGetWriter2d(handleDocumnet, &writerHandle, &errorInformation);
            }

            this.HandleErrorCases(returnCode, in errorInformation);
            return writerHandle;
        }

        public void DestroyWriter2d(IntPtr handleWriter)
        {
            this.ThrowIfNotInitialized();
            this.destroyWriter2d(handleWriter);
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

        public QueryResult Reader2dQuery(IntPtr read2dHandle, IDimensionQueryClause clause, ITileInfoQueryClause tileInfoQueryClause, int maxNumberOfResults)
        {
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
                            returnCode = this.idocread2dQuery(
                                read2dHandle,
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
                            returnCode = this.idocread2dQuery(
                                read2dHandle,
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
                            returnCode = this.idocread2dQuery(
                                read2dHandle,
                                IntPtr.Zero,
                                new IntPtr(pointerTileInfoQueryClauseInterop),
                                new IntPtr(pointerQueryResultInterop),
                                &errorInformation);
                        }
                    }
                    else
                    {
                        // if (dimensionQueryClauseInterop != null && tileInfoQueryClauseInterop != null)
                        returnCode = this.idocread2dQuery(
                            read2dHandle,
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

        public byte[] Reader2dReadTileData(IntPtr read2dHandle, long pk)
        {
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
                        logicalPosition = ConvertToLogicalPosition(logicalPositionInfoInterop);
                    }

                    if (fillTileBlobInfo)
                    {
                        tileBlobInfo = ConvertToTileBlobInfo(tileBlobInfoInterop);
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
                        logicalPosition = ConvertToLogicalPosition(logicalPositionInfoInterop);
                    }

                    if (fillTileBlobInfo)
                    {
                        tileBlobInfo = ConvertToTileBlobInfo(tileBlobInfoInterop);
                    }
                }
            }
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

    /// <content> 
    /// Here we gather "simple overloads" of the interface functions.
    /// </content>
    public partial class ImgDoc2ApiInterop
    {
        public void Writer2dAddTile(IntPtr write2dHandle, ITileCoordinate coordinate, in LogicalPosition logicalPosition, Tile2dBaseInfo tile2dBaseInfo, DataType dataType, byte[] data)
        {
            if (data != null && data.Length > 0)
            {
                unsafe
                {
                    fixed (byte* pointer = &data[0])
                    {
                        this.Writer2dAddTile(write2dHandle, coordinate, in logicalPosition, tile2dBaseInfo, dataType, new IntPtr(pointer), data.Length);
                    }
                }
            }
            else
            {
                this.Writer2dAddTile(write2dHandle, coordinate, in logicalPosition, tile2dBaseInfo, dataType, IntPtr.Zero, 0);
            }
        }
    }

    /// <content> 
    /// The public class "QueryResult" is defined in this part.
    /// </content>
    public partial class ImgDoc2ApiInterop
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
    public partial class ImgDoc2ApiInterop
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
        /// Function pointer (callable from unmanaged code) to the function "BlobOutputSetSizeFunction".
        /// </summary>
        private readonly IntPtr funcPtrBlobOutputSetSizeForwarder;

        /// <summary>
        /// Function pointer (callable from unmanaged code) to the function "BlobOutputSetDataFunction".
        /// </summary>
        private readonly IntPtr funcPtrBlobOutputSetDataForwarder;

        private delegate bool BlobOutputSetSizeDelegate(IntPtr blobOutputObjectHandle, ulong size);

        private delegate bool BlobOutputSetDataDelegate(IntPtr blobOutputObjectHandle, ulong offset, ulong size, IntPtr pointerToData);

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
    public partial class ImgDoc2ApiInterop
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

    public partial class ImgDoc2ApiInterop
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
    public partial class ImgDoc2ApiInterop
    {
        private const int ImgDoc2ErrorInformationMessageMaxLength = 200;

#pragma warning disable SA1310 // Field names should not contain underscore
        private const int ImgDoc2_ErrorCode_OK = 0;
        private const int ImgDoc2_ErrorCode_InvalidArgument = 1;
        private const int ImgDoc2_ErrorCode_UnspecifiedError = 50;
#pragma warning restore SA1310 // Field names should not contain underscore

        private readonly GetStatisticsDelegate getStatistics;
        private readonly VoidAndReturnIntPtrDelegate createCreateOptions;
        private readonly IntPtrAndReturnVoidDelegate destroyCreateOptions;

        private readonly VoidAndReturnIntPtrDelegate createOpenExistingOptions;
        private readonly IntPtrAndReturnVoidDelegate destroyOpenExistingOptions;

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
        private readonly IntPtrAndReturnVoidDelegate destroyDocument;

        private readonly IDoc_GetObjectDelegate documentGetReader2d;
        private readonly IntPtrAndReturnVoidDelegate destroyReader2d;
        private readonly IDoc_GetObjectDelegate documentGetWriter2d;
        private readonly IntPtrAndReturnVoidDelegate destroyWriter2d;

        private readonly IDocWrite2d_AddTileDelegate idocwrite2dAddTile;
        private readonly IDocRead2d_QueryDelegate idocread2dQuery;
        private readonly IDocRead2d_GetTilesIntersectingRectDelegate idocread2dGetTilesIntersectingRect;
        private readonly IDocRead2d_ReadTileDataDelegate idocread2dReadTileData;
        private readonly IDocRead2d_ReadTileInfoDelegate idocread2ReadTileInfo;

        private readonly CreateEnvironmentObjectDelegate createEnvironmentObject;

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate void GetStatisticsDelegate(ImgDoc2StatisticsInterop* statisticsInterop);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate IntPtr VoidAndReturnIntPtrDelegate();

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int IntPtrAndReturnVoidDelegate(IntPtr handle);

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

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int IDocRead2d_QueryDelegate(
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
        private unsafe delegate IntPtr CreateEnvironmentObjectDelegate(
            IntPtr userParameter,
            IntPtr pfnLog,
            IntPtr pfnIsLevelActive,
            IntPtr reportFatalErrorAndExit);

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

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        private struct ImgDoc2StatisticsInterop
        {
            public uint NumberOfCreateOptionsObjectsActive;
            public uint NumberOfOpenExistingOptionsObjectsActive;
            public uint NumberOfDocumentObjectsActive;
            public uint NumberOfReader2dObjectsActive;
            public uint NumberOfWriter2dObjectsActive;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        private struct RectangleDoubleInterop
        {
            public double X;
            public double Y;
            public double Width;
            public double Height;
        }
    }

    /// <summary>   
    /// This part contains the declaration of the delegates and native structs. 
    /// </summary>
    public partial class ImgDoc2ApiInterop
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
            QueryResult queryResult = new QueryResult(count);
            queryResult.ResultComplete = BitConverter.ToInt32(queryResultInterop, 4) == 0 ? true : false;

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
            tileBaseInfoInterop.PixelType = 0;
            return tileBaseInfoInterop;
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

        private static Tile2dBaseInfo ConvertToTile2dBaseInfo(in TileBaseInfoInterop tileBaseInfoInterop)
        {
            Tile2dBaseInfo tile2dBaseInfo = new Tile2dBaseInfo(
                (int)tileBaseInfoInterop.PixelWidth,
                (int)tileBaseInfoInterop.PixelHeight,
                (PixelType)tileBaseInfoInterop.PixelType); // TODO: check conversion to int and enum
            return tile2dBaseInfo;
        }

        private static TileBlobInfo ConvertToTileBlobInfo(in TileBlobInfoInterop tileBlobInfoInterop)
        {
            TileBlobInfo tileBlobInfo = new TileBlobInfo
            {
                Tile2dBaseInfo = ConvertToTile2dBaseInfo(tileBlobInfoInterop.TileBaseInfo),
                DataType = (DataType)tileBlobInfoInterop.DataType, // TODO: check enum
            };

            return tileBlobInfo;
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
    }
}