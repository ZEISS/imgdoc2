// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include "ConvCZI_Config.h"
#include <iostream>
#include <string>
#include <chrono>
#include <imgdoc2.h>
#include "utilities.h"
#include <libCZI.h>
#include "commandlineoptions.h"

using namespace std;
using namespace libCZI;
using namespace imgdoc2;

static shared_ptr<ICZIReader> CreateCziReader(const CmdlineOpts& options)
{
    auto spReader = libCZI::CreateCZIReader();
    auto stream = CreateStreamFromFile(convertToWide(options.GetCziFilename()).c_str());

    try
    {
        spReader->Open(stream);
    }
    catch (exception& ex)
    {
        stringstream ss;
        ss << "Could not open the CZI-file : " << ex.what();
        cerr << ss.str() << endl;
        return {};
    }

    return spReader;
}

static void ConvertDimCoordinate(const CDimCoordinate& dimCoordinate, TileCoordinate& tc)
{
    dimCoordinate.EnumValidDimensions(
        [&](DimensionIndex dim, int v)->bool
        {
            if (dim != DimensionIndex::B)
            {
                tc.Set(Utils::DimensionToChar(dim), v);
            }

            return true;
        });
}

/// Calculates the "pyramid layer no" from the specified subblock. 
/// Note that this operation is "problematic", to say the least. One of the complications
/// here is that in CZI there is no concept of "pyramid-layer", it is a concept of "a set of
/// tiles with arbitrary logical/physical sizes", so - there is no guarantee that the subblocks
/// (from the CZI side) can even be grouped into "pyramid-layers" at all.
/// \param  logicalRect                The logical rectangle.
/// \param  physicalSize               Size of the physical.
/// \param  minificationFactorPerLayer The minification factor per layer.
/// \returns The calculated pyramid layer no.
static int CalcPyramidLayerNo(const libCZI::IntRect& logicalRect, const libCZI::IntSize& physicalSize, int minificationFactorPerLayer)
{
    double minFactor;
    if (physicalSize.w > physicalSize.h)
    {
        minFactor = static_cast<double>(logicalRect.w) / physicalSize.w;
    }
    else
    {
        minFactor = static_cast<double>(logicalRect.h) / physicalSize.h;
    }

    int minFactorInt = static_cast<int>(round(minFactor));
    int f = 1;
    int layerNo = -1;
    for (int layer = 0;; layer++)
    {
        if (f >= minFactorInt)
        {
            layerNo = layer;
            break;
        }

        f *= minificationFactorPerLayer;
    }

    return layerNo;
}

/// Derives "TileBaseInfo" from the specified subblock-information.
/// \param  sbBlkInfo The libCZI-subblock-information structure.
/// \returns Tile-base-information describing tile.
static TileBaseInfo DeriveTileBaseInfo(const SubBlockInfo& sbBlkInfo)
{
    TileBaseInfo tile_base_info;
    switch (sbBlkInfo.pixelType)
    {
    case libCZI::PixelType::Bgr24:
        tile_base_info.pixelType = imgdoc2::PixelType::Bgr24;
        break;
    case libCZI::PixelType::Bgr48:
        tile_base_info.pixelType = imgdoc2::PixelType::Bgr48;;
        break;
    case libCZI::PixelType::Gray8:
        tile_base_info.pixelType = imgdoc2::PixelType::Gray8;
        break;
    case libCZI::PixelType::Gray16:
        tile_base_info.pixelType = imgdoc2::PixelType::Gray16;
        break;
    default:
        tile_base_info.pixelType = imgdoc2::PixelType::Unknown;
        break;
    }

    tile_base_info.pixelWidth = sbBlkInfo.physicalSize.w;
    tile_base_info.pixelHeight = sbBlkInfo.physicalSize.h;
    return tile_base_info;
}

static imgdoc2::DataTypes DetermineTileStorageDataType(const libCZI::ISubBlock* sub_block)
{
    switch (sub_block->GetSubBlockInfo().GetCompressionMode())
    {
    case CompressionMode::UnCompressed:
        return imgdoc2::DataTypes::UNCOMPRESSED_BITMAP;
    case CompressionMode::JpgXr:
        return imgdoc2::DataTypes::JPGXRCOMPRESSED_BITMAP;
    default:
        throw invalid_argument("Unsupported compression encountered.");
    }
}

/// Wrapper which is implementing the IDataObjBase-interface on an
/// libCZI-subblock-object.
class DataObjOnSubBlk : public IDataObjBase
{
private:
    const shared_ptr<ISubBlock>& sbBlk;
public:
    explicit DataObjOnSubBlk(const shared_ptr<ISubBlock>& sbblk) :sbBlk(sbblk)
    {
    }

    virtual void GetData(const void** p, size_t* s) const
    {
        const void* ptr; size_t size;
        this->sbBlk->DangerousGetRawData(ISubBlock::Data, ptr, size);
        *p = ptr;
        *s = size;
    }
};

int main(int argc, char** argv)
{
    CmdlineOpts cmdline_options;
    bool can_run_operation;
    try
    {
#if CONVCZI_WIN32_ENVIRONMENT
        CommandlineArgsWindowsHelper args_helper;
        can_run_operation = cmdline_options.ParseArguments(args_helper.GetArgc(), args_helper.GetArgv());
#endif
#if CONVCZI_UNIX_ENVIRONMENT
        setlocale(LC_CTYPE, "C.UTF-8");
        can_run_operation = cmdline_options.ParseArguments(argc, argv);
#endif
    }
    catch (exception&)
    {
        return EXIT_FAILURE;
    }

    if (!can_run_operation)
    {
        return EXIT_FAILURE;
    }

    auto czi_reader = CreateCziReader(cmdline_options);
    if (!czi_reader)
    {
        return EXIT_FAILURE;
    }

    // create the "imgdoc2-create-options"-object
    auto imgdoc2_create_options = ClassFactory::CreateCreateOptionsUp();

    // set parameters with the option-object
    imgdoc2_create_options->SetFilename(cmdline_options.GetDstFilename().c_str());
    imgdoc2_create_options->SetCreateBlobTable(true);

    auto subBlkStatistics = czi_reader->GetStatistics();
    subBlkStatistics.dimBounds.EnumValidDimensions(
        [&](DimensionIndex dim, int start, int end)->bool
        {
            if (dim != DimensionIndex::B)
            {
                char dimension = Utils::DimensionToChar(dim);
                imgdoc2_create_options->AddDimension(dimension);
            }

            return true;
        });

    // if there is a valid M-index, then we want to add a "m-dimension"
    bool includeMindex = false;
    if (subBlkStatistics.IsMIndexValid())
    {
        imgdoc2_create_options->AddDimension('M');
        includeMindex = true;
    }

    // now, with this options-object, we can create an imgdoc2-document
    auto imgdoc2_hosting_environment = ClassFactory::CreateStandardHostingEnvironment();
    shared_ptr<imgdoc2::IDoc> imgdoc2_document;
    try
    {
        imgdoc2_document = ClassFactory::CreateNew(
            imgdoc2_create_options.get(),
            imgdoc2_hosting_environment);
    }
    catch (database_exception& exception)
    {
        cerr << "Error creating output-document : errorcode=" <<
            (exception.GetIsSqliteErrorCodeValid() ? to_string(exception.GetSqliteErrorCode()) : "<invalid>") << endl;
        if (exception.GetIsSqliteErrorCodeValid())
        {
            cerr << " (" << exception.GetSqliteErrorMessage() << ")" << endl;
        }

        return EXIT_FAILURE;
    }
    catch (exception& exception)
    {
        cerr << "Error creating output-document : " << exception.what() << endl;
        return EXIT_FAILURE;
    }

    // ...from which we request the "writer2d-interface"
    auto imgdoc2_document_writer = imgdoc2_document->GetWriter2d();

    if (cmdline_options.GetMode() == CmdlineOpts::AddMode::SingleTransaction)
    {
        imgdoc2_document_writer->BeginTransaction();
    }

    cout << endl;

    // ... and now, we simply loop over every subblock in the CZI-file, and copy it over into the
    //      imgdoc2-document
    TileCoordinate imgdoc2_tile_coordinate;
    LogicalPositionInfo imgdoc2_logical_position_info;
    uint64_t total_data_size = 0;
    auto start = chrono::high_resolution_clock::now();
    uint32_t subblocks_processed_count = 0;
    uint32_t total_subblock_count = static_cast<uint32_t>(czi_reader->GetStatistics().subBlockCount);
    czi_reader->EnumerateSubBlocks(
        [&](int idx, const SubBlockInfo& info)->bool
        {
            imgdoc2_tile_coordinate.Clear();
            ConvertDimCoordinate(info.coordinate, imgdoc2_tile_coordinate);
            if (includeMindex)
            {
                imgdoc2_tile_coordinate.Set('M', info.mIndex);
            }

            imgdoc2_logical_position_info.posX = info.logicalRect.x;
            imgdoc2_logical_position_info.posY = info.logicalRect.y;
            imgdoc2_logical_position_info.width = info.logicalRect.w;
            imgdoc2_logical_position_info.height = info.logicalRect.h;
            imgdoc2_logical_position_info.pyrLvl = CalcPyramidLayerNo(info.logicalRect, info.physicalSize, 2);

            TileBaseInfo imgdoc2_tile_base_info = DeriveTileBaseInfo(info);

            auto sbBlk = czi_reader->ReadSubBlock(idx);
            const imgdoc2::DataTypes tile_storage_data_type = DetermineTileStorageDataType(sbBlk.get());

            DataObjOnSubBlk dataobj(sbBlk);
            imgdoc2_document_writer->AddTile(
                &imgdoc2_tile_coordinate,
                &imgdoc2_logical_position_info,
                &imgdoc2_tile_base_info,
                tile_storage_data_type,
                TileDataStorageType::BlobInDatabase,
                &dataobj);

            size_t size_of_subblock_data;
            const void* dummy;
            sbBlk->DangerousGetRawData(ISubBlock::MemBlkType::Data, dummy, size_of_subblock_data);
            total_data_size += size_of_subblock_data;

            cout << subblocks_processed_count << " / " << total_subblock_count << "\r";
            ++subblocks_processed_count;

            return true;
        });

    cout << endl;

    if (cmdline_options.GetMode() == CmdlineOpts::AddMode::SingleTransaction)
    {
        imgdoc2_document_writer->CommitTransaction();
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed_seconds = end - start;

    cout << "Operation completed within " << elapsed_seconds.count() << "s -> datarate=" << total_data_size / elapsed_seconds.count() / 1e6 << "MB/s" << endl;

    // and... done
    return EXIT_SUCCESS;
}
