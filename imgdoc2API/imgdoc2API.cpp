// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include "imgdoc2API.h"
#include <memory>
#include <algorithm>
#include <atomic>
#include <utility>
#include <string>
#include <vector>
#include "utilities.h"
#include <imgdoc2.h>

using namespace imgdoc2;
using namespace std;

struct ImgDoc2ApiStatistics
{
    atomic_uint32_t number_of_createoptions_objects_active{ 0 };
    atomic_uint32_t number_of_openexistingoptions_objects_active{ 0 };
    atomic_uint32_t number_of_document_objects_active{ 0 };
    atomic_uint32_t number_of_reader2d_objects_active{ 0 };
    atomic_uint32_t number_of_writer2d_objects_active{ 0 };

    ImgDoc2StatisticsInterop GetInteropStruct()
    {
        ImgDoc2StatisticsInterop interop;
        interop.number_of_createoptions_objects_active = this->number_of_createoptions_objects_active.load();
        interop.number_of_openexistingoptions_objects_active = this->number_of_openexistingoptions_objects_active.load();
        interop.number_of_document_objects_active = this->number_of_document_objects_active.load();
        interop.number_of_reader2d_objects_active = this->number_of_reader2d_objects_active.load();
        interop.number_of_writer2d_objects_active = this->number_of_writer2d_objects_active.load();
        return interop;
    }
};

static ImgDoc2ApiStatistics gImgDoc2ApiStatistics;

template <typename t>
struct SharedPtrWrapper
{
    explicit SharedPtrWrapper(std::shared_ptr<t> shared_ptr) : shared_ptr_(std::move(shared_ptr)) {}
    std::shared_ptr<t> shared_ptr_;
};

static void FillOutErrorInformation(const exception& exception, ImgDoc2ErrorInformation* error_information)
{
    if (error_information != nullptr)
    {
        const auto error_message = exception.what();

        // ensure that the string is always null-terminated, even in the case of truncation
        Utilities::copy_string_to_fixed_size(error_message, error_information->message, ImgDoc2ErrorInformation::kMaxMessageLength);
    }
}

static void FillOutErrorInformationForInvalidArgument(const char* argument_name, const char* text, ImgDoc2ErrorInformation* error_information)
{
    if (error_information != nullptr)
    {
        stringstream ss;
        ss << "Argument '" << argument_name << "'";
        if (text != nullptr)
        {
            ss << ": " << text;
        }

        Utilities::copy_string_to_fixed_size(ss.str().c_str(), error_information->message, ImgDoc2ErrorInformation::kMaxMessageLength);
    }
}

static ImgDoc2ErrorCode MapExceptionToReturnValue(const exception& exception)
{
    if (typeid(exception) == typeid(invalid_argument))
    {
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    if (typeid(exception) == typeid(non_existing_tile_exception))
    {
        return ImgDoc2_Invalid_TileId;
    }

    // TODO(Jbl) - add additional exception types

    return ImgDoc2_ErrorCode_UnspecifiedError;
}

void GetStatistics(ImgDoc2StatisticsInterop* statistics_interop)
{
    if (statistics_interop != nullptr)
    {
        *statistics_interop = gImgDoc2ApiStatistics.GetInteropStruct();
    }
}

HandleEnvironmentObject CreateEnvironmentObject(
    std::intptr_t user_parameter,
    void (*pfn_log)(std::intptr_t userparam, int level, const char* szMessage),
    bool (*pfn_is_level_active)(std::intptr_t userparam, int level),
    void (*pfn_report_fatal_error_and_exit)(std::intptr_t userparam, const char* szMessage))
{
    const auto environment = ClassFactory::CreateHostingEnvironmentForFunctionPointers(
        user_parameter,
        pfn_log,
        pfn_is_level_active,
        pfn_report_fatal_error_and_exit);
    auto shared_environment_wrappping_object = new SharedPtrWrapper<IHostingEnvironment>{ environment };
    return reinterpret_cast<HandleEnvironmentObject>(shared_environment_wrappping_object);
}

void DestroyEnvironmentObject(HandleEnvironmentObject handle)
{
    const auto object = reinterpret_cast<SharedPtrWrapper<IHostingEnvironment>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    delete object;
}

HandleCreateOptions CreateCreateOptions()
{
    ++gImgDoc2ApiStatistics.number_of_createoptions_objects_active;
    return reinterpret_cast<HandleCreateOptions>(ClassFactory::CreateCreateOptionsPtr());
}

void DestroyCreateOptions(HandleCreateOptions handle)
{
    const auto object = reinterpret_cast<ICreateOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    delete object;
    --gImgDoc2ApiStatistics.number_of_createoptions_objects_active;
}

HandleOpenExistingOptions CreateOpenExistingOptions()
{
    ++gImgDoc2ApiStatistics.number_of_openexistingoptions_objects_active;
    return reinterpret_cast<HandleOpenExistingOptions>(ClassFactory::CreateOpenExistingOptions());
}

void DestroyOpenExistingOptions(HandleOpenExistingOptions handle)
{
    const auto object = reinterpret_cast<IOpenExistingOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    delete object;
    --gImgDoc2ApiStatistics.number_of_openexistingoptions_objects_active;
}

ImgDoc2ErrorCode CreateNewDocument(HandleCreateOptions create_options, HandleEnvironmentObject handle_environment_object, HandleDoc* document, ImgDoc2ErrorInformation* error_information)
{
    if (document == nullptr)
    {
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    shared_ptr<imgdoc2::IDoc> imgdoc2;
    shared_ptr<imgdoc2::IHostingEnvironment> hosting_environment;
    if (handle_environment_object != kInvalidObjectHandle)
    {
        hosting_environment = reinterpret_cast<SharedPtrWrapper<IHostingEnvironment>*>(handle_environment_object)->shared_ptr_;
    }

    try
    {
        imgdoc2 = ClassFactory::CreateNew(reinterpret_cast<ICreateOptions*>(create_options), hosting_environment);  // NOLINT(performance-no-int-to-ptr)
    }
    catch (exception& exception)
    {
        FillOutErrorInformation(exception, error_information);
        return MapExceptionToReturnValue(exception);
    }

    auto shared_imgdoc_wrappping_object = new SharedPtrWrapper<IDoc>{ imgdoc2 };
    *document = reinterpret_cast<HandleDoc>(shared_imgdoc_wrappping_object);
    ++gImgDoc2ApiStatistics.number_of_document_objects_active;
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode OpenExistingDocument(
    HandleOpenExistingOptions open_existing_options,
    HandleEnvironmentObject handle_environment_object,
    HandleDoc* document,
    ImgDoc2ErrorInformation* error_information)
{
    if (document == nullptr)
    {
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    shared_ptr<imgdoc2::IDoc> imgdoc2;
    shared_ptr<imgdoc2::IHostingEnvironment> hosting_environment;
    if (handle_environment_object != kInvalidObjectHandle)
    {
        hosting_environment = reinterpret_cast<SharedPtrWrapper<IHostingEnvironment>*>(handle_environment_object)->shared_ptr_; // NOLINT(performance-no-int-to-ptr)
    }

    try
    {
        imgdoc2 = ClassFactory::OpenExisting(reinterpret_cast<IOpenExistingOptions*>(open_existing_options), hosting_environment);  // NOLINT(performance-no-int-to-ptr)
    }
    catch (exception& exception)
    {
        FillOutErrorInformation(exception, error_information);
        return MapExceptionToReturnValue(exception);
    }

    auto shared_imgdoc_wrappping_object = new SharedPtrWrapper<IDoc>{ imgdoc2 };
    *document = reinterpret_cast<HandleDoc>(shared_imgdoc_wrappping_object);
    ++gImgDoc2ApiStatistics.number_of_document_objects_active;
    return ImgDoc2_ErrorCode_OK;
}

void DestroyDocument(HandleDoc handle)
{
    const auto object = reinterpret_cast<SharedPtrWrapper<IDoc>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    delete object;
    --gImgDoc2ApiStatistics.number_of_document_objects_active;
}

ImgDoc2ErrorCode IDoc_GetReader2d(HandleDoc handle_document, HandleDocRead2D* reader, ImgDoc2ErrorInformation* error_information)
{
    if (reader == nullptr)
    {
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    auto spReader2d = reinterpret_cast<SharedPtrWrapper<IDoc>*>(handle_document)->shared_ptr_->GetReader2d();   // NOLINT(performance-no-int-to-ptr)
    if (spReader2d)
    {
        auto shared_reader2d_wrappping_object = new SharedPtrWrapper<IDocRead2d>{ spReader2d };
        *reader = reinterpret_cast<HandleDocRead2D>(shared_reader2d_wrappping_object);
        ++gImgDoc2ApiStatistics.number_of_reader2d_objects_active;
    }
    else
    {
        *reader = kInvalidObjectHandle;
    }

    return ImgDoc2_ErrorCode_OK;
}

void DestroyReader2d(HandleDocRead2D handle)
{
    const auto object = reinterpret_cast<SharedPtrWrapper<IDocRead2d>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    delete object;
    --gImgDoc2ApiStatistics.number_of_reader2d_objects_active;
}

ImgDoc2ErrorCode IDoc_GetWriter2d(HandleDoc handle_document, HandleDocWrite2D* document_writer2d, ImgDoc2ErrorInformation* error_information)
{
    if (document_writer2d == nullptr)
    {
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    auto spWriter2d = reinterpret_cast<SharedPtrWrapper<IDoc>*>(handle_document)->shared_ptr_->GetWriter2d();   // NOLINT(performance-no-int-to-ptr)
    if (spWriter2d)
    {
        auto shared_writer2d_wrappping_object = new SharedPtrWrapper<IDocWrite2d>{ spWriter2d };
        *document_writer2d = reinterpret_cast<HandleDocWrite2D>(shared_writer2d_wrappping_object);
        ++gImgDoc2ApiStatistics.number_of_writer2d_objects_active;
    }
    else
    {
        *document_writer2d = kInvalidObjectHandle;
    }

    return ImgDoc2_ErrorCode_OK;
}

void DestroyWriter2d(HandleDocWrite2D handle)
{
    const auto object = reinterpret_cast<SharedPtrWrapper<IDocWrite2d>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    delete object;
    --gImgDoc2ApiStatistics.number_of_writer2d_objects_active;
}

ImgDoc2ErrorCode CreateOptions_SetFilename(HandleCreateOptions handle, const char* filename_utf8, ImgDoc2ErrorInformation* error_information)
{
    const auto object = reinterpret_cast<ICreateOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    object->SetFilename(filename_utf8);
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode OpenExistingOptions_SetFilename(HandleOpenExistingOptions handle, const char* filename_utf8, ImgDoc2ErrorInformation* error_information)
{
    const auto object = reinterpret_cast<IOpenExistingOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    object->SetFilename(filename_utf8);
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_SetUseSpatialIndex(HandleCreateOptions handle, bool use_spatial_index, ImgDoc2ErrorInformation* error_information)
{
    const auto object = reinterpret_cast<ICreateOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    object->SetUseSpatialIndex(use_spatial_index);
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_SetUseBlobTable(HandleCreateOptions handle, bool use_blob_table, ImgDoc2ErrorInformation* error_information)
{
    const auto object = reinterpret_cast<ICreateOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    object->SetCreateBlobTable(use_blob_table);
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_AddIndexForDimension(HandleCreateOptions handle, char dimension, ImgDoc2ErrorInformation* error_information)
{
    const auto object = reinterpret_cast<ICreateOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    object->AddIndexForDimension(dimension);
    return ImgDoc2_ErrorCode_OK;
}

template <typename tGetString>
static ImgDoc2ErrorCode ReturnStringHelper(const tGetString& getString, char* filename_utf8, size_t* size, ImgDoc2ErrorInformation* error_information)
{
    if (size == nullptr || *size < 1)
    {
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    const string& filename = getString();
    if (filename_utf8 != nullptr)
    {
        memcpy(filename_utf8, filename.c_str(), min(*size, 1 + filename.length()));
        filename_utf8[*size - 1] = '\0';
    }

    *size = 1 + filename.length();
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_GetFilename(HandleCreateOptions handle, char* filename_utf8, size_t* size, ImgDoc2ErrorInformation* error_information)
{
    return ReturnStringHelper(
        [=]()->std::string
        {
            const auto object = reinterpret_cast<ICreateOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    return object->GetFilename();
        },
        filename_utf8,
            size,
            error_information);
}

ImgDoc2ErrorCode OpenExistingOptions_GetFilename(HandleOpenExistingOptions handle, char* filename_utf8, size_t* size, ImgDoc2ErrorInformation* error_information)
{
    return ReturnStringHelper(
        [=]()->std::string
        {
            const auto object = reinterpret_cast<IOpenExistingOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    return object->GetFilename();
        },
        filename_utf8,
            size,
            error_information);
}

ImgDoc2ErrorCode CreateOptions_GetUseSpatialIndex(HandleCreateOptions handle, bool* use_spatial_index, ImgDoc2ErrorInformation* error_information)
{
    const auto object = reinterpret_cast<ICreateOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    const bool b = object->GetUseSpatialIndex();
    if (use_spatial_index != nullptr)
    {
        *use_spatial_index = b;
    }

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_GetUseBlobTable(HandleCreateOptions handle, bool* use_blob_table, ImgDoc2ErrorInformation* error_information)
{
    const auto object = reinterpret_cast<ICreateOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    const bool b = object->GetCreateBlobTable();
    if (use_blob_table != nullptr)
    {
        *use_blob_table = b;
    }

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_AddDimension(HandleCreateOptions handle, std::uint8_t  dimension, ImgDoc2ErrorInformation* error_information)
{
    const auto object = reinterpret_cast<ICreateOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    try
    {
        object->AddDimension(dimension);
    }
    catch (exception& exception)
    {
        FillOutErrorInformation(exception, error_information);
        return MapExceptionToReturnValue(exception);
    }

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_AddIndexedDimension(HandleCreateOptions handle, std::uint8_t  dimension, ImgDoc2ErrorInformation* error_information)
{
    const auto object = reinterpret_cast<ICreateOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    try
    {
        object->AddIndexForDimension(dimension);
    }
    catch (exception& exception)
    {
        FillOutErrorInformation(exception, error_information);
        return MapExceptionToReturnValue(exception);
    }

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_GetDimensions(HandleCreateOptions handle, std::uint8_t* dimensions, size_t* elements_count, ImgDoc2ErrorInformation* error_information)
{
    if (elements_count == nullptr)
    {
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    auto* const object = reinterpret_cast<ICreateOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    const auto dimensions_from_object = object->GetDimensions();

    size_t count = 0;
    for (auto d : dimensions_from_object)
    {
        if (count >= *elements_count)
        {
            break;
        }

        dimensions[count++] = d;
    }

    *elements_count = dimensions_from_object.size();
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_GetIndexedDimensions(HandleCreateOptions handle, std::uint8_t* dimensions, size_t* elements_count, ImgDoc2ErrorInformation* error_information)
{
    if (elements_count == nullptr)
    {
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    auto* const object = reinterpret_cast<ICreateOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    const auto dimensions_from_object = object->GetIndexedDimensions();

    size_t count = 0;
    for (auto d : dimensions_from_object)
    {
        if (count >= *elements_count)
        {
            break;
        }

        dimensions[count++] = d;
    }

    *elements_count = dimensions_from_object.size();
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode IDocWrite2d_AddTile(
    HandleDocWrite2D handle,
    const TileCoordinateInterop* tile_coordinate_interop,
    const LogicalPositionInfoInterop* logical_position_info_interop,
    const TileBaseInfoInterop* tile_base_info_interop,
    std::uint8_t data_type_interop,
    const void* ptr_data,
    std::uint64_t size_data,
    imgdoc2::dbIndex* result_pk,
    ImgDoc2ErrorInformation* error_information)
{
    if (tile_coordinate_interop == nullptr)
    {
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    if (logical_position_info_interop == nullptr)
    {
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    auto tile_coordinate = Utilities::ConvertToTileCoordinate(tile_coordinate_interop);
    auto logical_position_info = Utilities::ConvertLogicalPositionInfoInteropToImgdoc2(*logical_position_info_interop);
    TileBaseInfo tile_info = Utilities::ConvertTileBaseInfoInteropToImgdoc2(*tile_base_info_interop);
    DataTypes data_type = Utilities::ConvertDatatypeEnumInterop(data_type_interop);

    auto writer2d = reinterpret_cast<SharedPtrWrapper<IDocWrite2d>*>(handle)->shared_ptr_; // NOLINT(performance-no-int-to-ptr)

    struct GetDataObject : public IDataObjBase
    {
    private:
        const void* p_;
        size_t s_;
    public:
        GetDataObject(const void* p, size_t s) :p_(p), s_(s) {}
        void GetData(const void** p, size_t* s) const override
        {
            if (p != nullptr)
            {
                *p = this->p_;
            }

            if (s != nullptr)
            {
                *s = this->s_;
            }
        }
    };

    try
    {
        const GetDataObject data_object(ptr_data, size_data);
        auto pk = writer2d->AddTile(
            &tile_coordinate,
            &logical_position_info,
            &tile_info,
            data_type,
            TileDataStorageType::BlobInDatabase,
            &data_object);
        if (result_pk != nullptr)
        {
            *result_pk = pk;
        }
    }
    catch (exception& exception)
    {
        FillOutErrorInformation(exception, error_information);
        return MapExceptionToReturnValue(exception);
    }


    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode IDocRead2d_Query(
    HandleDocRead2D handle,
    const DimensionQueryClauseInterop* dim_coordinate_query_clause_interop,
    const TileInfoQueryClauseInterop* tile_info_query_clause_interop,
    QueryResultInterop* result,
    ImgDoc2ErrorInformation* error_information)
{
    auto reader2d = reinterpret_cast<SharedPtrWrapper<IDocRead2d>*>(handle)->shared_ptr_; // NOLINT(performance-no-int-to-ptr)

    auto dimension_coordinate_query_clause = dim_coordinate_query_clause_interop != nullptr ?
        Utilities::ConvertDimensionQueryRangeClauseInteropToImgdoc2(dim_coordinate_query_clause_interop) :
        CDimCoordinateQueryClause();
    auto tile_info_query_clause = tile_info_query_clause_interop != nullptr ?
        Utilities::ConvertTileInfoQueryClauseInteropToImgdoc2(tile_info_query_clause_interop) :
        CTileInfoQueryClause();

    uint32_t results_retrieved_count = 0;
    result->more_results_available = 0;

    try
    {
        reader2d->Query(
            dim_coordinate_query_clause_interop != nullptr ? &dimension_coordinate_query_clause : nullptr,
            tile_info_query_clause_interop != nullptr ? &tile_info_query_clause : nullptr,
            [result, &results_retrieved_count](imgdoc2::dbIndex index)->bool
            {
                if (results_retrieved_count < result->element_count)
                {
                    result->indices[results_retrieved_count] = index;
                    ++results_retrieved_count;
                    return true;
                }

                result->more_results_available = 1;
                return false;
            });
    }
    catch (exception& exception)
    {
        FillOutErrorInformation(exception, error_information);
        return MapExceptionToReturnValue(exception);
    }

    result->element_count = results_retrieved_count;

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode IDocRead2d_GetTilesIntersectingRect(
    HandleDocRead2D handle,
    const RectangleDoubleInterop* query_rectangle,
    const DimensionQueryClauseInterop* dim_coordinate_query_clause_interop,
    const TileInfoQueryClauseInterop* tile_info_query_clause_interop,
    QueryResultInterop* result,
    ImgDoc2ErrorInformation* error_information)
{
    auto reader2d = reinterpret_cast<SharedPtrWrapper<IDocRead2d>*>(handle)->shared_ptr_; // NOLINT(performance-no-int-to-ptr)

    auto tile_info_query_clause = tile_info_query_clause_interop != nullptr ?
        Utilities::ConvertTileInfoQueryClauseInteropToImgdoc2(tile_info_query_clause_interop) :
        CTileInfoQueryClause();
    auto dimension_coordinate_query_clause = dim_coordinate_query_clause_interop != nullptr ?
        Utilities::ConvertDimensionQueryRangeClauseInteropToImgdoc2(dim_coordinate_query_clause_interop) :
        CDimCoordinateQueryClause();

    RectangleD rectangle = Utilities::ConvertRectangleDoubleInterop(*query_rectangle);
    uint32_t results_retrieved_count = 0;
    result->more_results_available = 0;

    try
    {
        reader2d->GetTilesIntersectingRect(
            rectangle,
            dim_coordinate_query_clause_interop != nullptr ? &dimension_coordinate_query_clause : nullptr,
            tile_info_query_clause_interop != nullptr ? &tile_info_query_clause : nullptr,
            [result, &results_retrieved_count](imgdoc2::dbIndex index)->bool
            {
                if (results_retrieved_count < result->element_count)
                {
                    result->indices[results_retrieved_count] = index;
                    ++results_retrieved_count;
                    return true;
                }

                result->more_results_available = 1;
                return false;
            });
    }
    catch (exception& exception)
    {
        FillOutErrorInformation(exception, error_information);
        return MapExceptionToReturnValue(exception);
    }

    result->element_count = results_retrieved_count;

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode IDocRead2d_ReadTileData(
    HandleDocRead2D handle,
    std::int64_t pk,
    std::intptr_t blob_output_handle,
    bool(LIBIMGDOC2_STDCALL* pfnReserve)(std::intptr_t /*blob_output_handle*/, std::uint64_t /*size*/), // NOLINT(readability/casting)
    bool(LIBIMGDOC2_STDCALL* pfnSetData)(std::intptr_t /*blob_output_handle*/, std::uint64_t /*offset*/, std::uint64_t /*size*/, const void* /*data*/), // NOLINT(readability/casting)
    ImgDoc2ErrorInformation* error_information)
{
    static_assert(sizeof(pk) == sizeof(imgdoc2::dbIndex), "Type of the argument 'pk' and the imgdoc2-dbIndex-type must have same size.");

    auto reader2d = reinterpret_cast<SharedPtrWrapper<IDocRead2d>*>(handle)->shared_ptr_; // NOLINT(performance-no-int-to-ptr)

    Utilities::BlobOutputOnFunctionsDecorator blob_output_object(blob_output_handle, pfnReserve, pfnSetData);

    try
    {
        reader2d->ReadTileData(pk, &blob_output_object);
    }
    catch (exception& exception)
    {
        FillOutErrorInformation(exception, error_information);
        return MapExceptionToReturnValue(exception);
    }

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode IDocRead2d_ReadTileInfo(
    HandleDocRead2D handle,
    std::int64_t pk,
    TileCoordinateInterop* tile_coordinate_interop,
    LogicalPositionInfoInterop* logical_position_info_interop,
    TileBlobInfoInterop* tile_blob_info_interop,
    ImgDoc2ErrorInformation* error_information)
{
    static_assert(sizeof(pk) == sizeof(imgdoc2::dbIndex), "Type of the argument 'pk' and the imgdoc2-dbIndex-type must have same size.");

    const auto reader2d = reinterpret_cast<SharedPtrWrapper<IDocRead2d>*>(handle)->shared_ptr_; // NOLINT(performance-no-int-to-ptr)

    LogicalPositionInfo logical_position_info;
    TileCoordinate tile_coordinate;
    TileBlobInfo tile_blob_info;

    try
    {
        reader2d->ReadTileInfo(
            pk,
            tile_coordinate_interop != nullptr ? &tile_coordinate : nullptr,
            logical_position_info_interop != nullptr ? &logical_position_info : nullptr,
            tile_blob_info_interop != nullptr ? &tile_blob_info : nullptr);
    }
    catch (exception& exception)
    {
        FillOutErrorInformation(exception, error_information);
        return MapExceptionToReturnValue(exception);
    }

    if (tile_coordinate_interop != nullptr)
    {
        const bool b = Utilities::TryConvertToTileCoordinateInterop(&tile_coordinate, tile_coordinate_interop);
        if (!b)
        {
            // TODO(JBL): implement error-handling
        }
    }

    if (logical_position_info_interop != nullptr)
    {
        *logical_position_info_interop = Utilities::ConvertImgDoc2LogicalPositionInfoToInterop(logical_position_info);
    }

    if (tile_blob_info_interop != nullptr)
    {
        *tile_blob_info_interop = Utilities::ConvertImgDoc2TileBlobInfoToInterop(tile_blob_info);
    }

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode IDocInfo_GetTileDimensions(
    HandleDocRead2D handle,
    imgdoc2::Dimension* dimensions,
    std::uint32_t* count,
    ImgDoc2ErrorInformation* error_information)
{
    if (count == nullptr)
    {
        FillOutErrorInformationForInvalidArgument("count", "must not be null", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    if (*count > 0 && dimensions == nullptr)
    {
        FillOutErrorInformationForInvalidArgument("dimensions", "must not be null (if a count > 0 was given)", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    const auto reader2d = reinterpret_cast<SharedPtrWrapper<IDocRead2d>*>(handle)->shared_ptr_; // NOLINT(performance-no-int-to-ptr)

    reader2d->GetTileDimensions(dimensions, *count);
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode IDocInfo_GetMinMaxForTileDimensions(
    HandleDocRead2D handle,
    const imgdoc2::Dimension* dimensions,
    std::uint32_t count,
    MinMaxForTilePositionsInterop* result,
    ImgDoc2ErrorInformation* error_information)
{
    if (dimensions == nullptr)
    {
        FillOutErrorInformationForInvalidArgument("dimensions", "must not be null", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    if (result == nullptr)
    {
        FillOutErrorInformationForInvalidArgument("result", "must not be null", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    const auto reader2d = reinterpret_cast<SharedPtrWrapper<IDocRead2d>*>(handle)->shared_ptr_; // NOLINT(performance-no-int-to-ptr)

    const vector<Dimension> dimensions_array(dimensions, dimensions + count);
    const auto min_max = reader2d->GetMinMaxForTileDimension(dimensions_array);

    for (uint32_t i = 0; i < count; ++i)
    {
        const auto& item = min_max.at(dimensions[i]);
        (result + i)->minimum_value = item.minimum_value;
        (result + i)->maximum_value = item.maximum_value;
    }

    return ImgDoc2_ErrorCode_OK;
}
