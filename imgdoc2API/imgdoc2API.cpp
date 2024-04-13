// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include "imgdoc2API.h"
#include <memory>
#include <algorithm>
#include <atomic>
#include <map>
#include <limits>
#include <utility>
#include <string>
#include <vector>
#include <gsl/util>
#include "utilities.h"
#include "imgdoc2apistatistics.h"
#include "sharedptrwrapper.h"
#include "imgdoc2APIsupport.h"

#include <imgdoc2.h>
#include <gsl/narrow>

using namespace imgdoc2;
using namespace std;

static ImgDoc2ApiStatistics g_imgdoc2_api_statistics;  ///< Define a static object, which is used to count active instances of objects, which are created by the imgdoc2API.

void GetStatistics(ImgDoc2StatisticsInterop* statistics_interop)
{
    if (statistics_interop != nullptr)
    {
        *statistics_interop = g_imgdoc2_api_statistics.GetInteropStruct();
    }
}

static void ClearAllocationObject(AllocationObject* allocation_object)
{
    allocation_object->pointer_to_memory = nullptr;
    allocation_object->handle = numeric_limits<intptr_t>::max();
}

static void CopyStringToAllocationObject(const string& str, AllocMemoryFunctionPointer allocate_memory_function, AllocationObject* allocation_object)
{
    // get the length of the string including the null terminator
    const size_t size = str.length() + 1;

    if (allocate_memory_function != nullptr &&
            allocate_memory_function(size, allocation_object) &&
            allocation_object->pointer_to_memory != nullptr)
    {
        memcpy(allocation_object->pointer_to_memory, str.c_str(), size);
    }
    else
    {
        ClearAllocationObject(allocation_object);
    }
}

ImgDoc2ErrorCode GetVersionInfo(VersionInfoInterop* version_info, AllocMemoryFunctionPointer allocate_memory_function)
{
    if (version_info == nullptr)
    {
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    const VersionInfo native_version_info = ClassFactory::GetVersionInfo();
    try
    {
        version_info->major = gsl::narrow<int32_t>(native_version_info.major);
        version_info->minor = gsl::narrow<int32_t>(native_version_info.minor);
        version_info->revision = gsl::narrow<int32_t>(native_version_info.patch);
    }
    catch (const gsl::narrowing_error&)
    {
        return ImgDoc2_ErrorCode_UnspecifiedError;
    }

    CopyStringToAllocationObject(native_version_info.compiler_identification, allocate_memory_function, &version_info->compiler_identification);
    CopyStringToAllocationObject(native_version_info.build_type, allocate_memory_function, &version_info->build_type);
    CopyStringToAllocationObject(native_version_info.repository_url, allocate_memory_function, &version_info->repository_url);
    CopyStringToAllocationObject(native_version_info.repository_branch, allocate_memory_function, &version_info->repository_branch);
    CopyStringToAllocationObject(native_version_info.repository_tag, allocate_memory_function, &version_info->repository_tag);
    return ImgDoc2_ErrorCode_OK;
}

HandleEnvironmentObject CreateEnvironmentObject(
    std::intptr_t user_parameter,
    void (LIBIMGDOC2_STDCALL* pfn_log)(std::intptr_t userparam, int level, const char* szMessage),
    bool (LIBIMGDOC2_STDCALL* pfn_is_level_active)(std::intptr_t userparam, int level),
    void (LIBIMGDOC2_STDCALL* pfn_report_fatal_error_and_exit)(std::intptr_t userparam, const char* szMessage))
{
    const auto environment = ClassFactory::CreateHostingEnvironmentForFunctionPointers(
        user_parameter,
        pfn_log,
        pfn_is_level_active,
        pfn_report_fatal_error_and_exit);
    auto shared_environment_wrapping_object = new SharedPtrWrapper<IHostingEnvironment>{ environment };
    return reinterpret_cast<HandleEnvironmentObject>(shared_environment_wrapping_object);
}

ImgDoc2ErrorCode DestroyEnvironmentObject(HandleEnvironmentObject handle, ImgDoc2ErrorInformation* error_information)
{
    const auto object = reinterpret_cast<SharedPtrWrapper<IHostingEnvironment>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    if (!object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleEnvironmentObject", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    delete object;
    return ImgDoc2_ErrorCode_OK;
}

HandleCreateOptions CreateCreateOptions()
{
    ++g_imgdoc2_api_statistics.number_of_createoptions_objects_active;
    return reinterpret_cast<HandleCreateOptions>(new PtrWrapper(ClassFactory::CreateCreateOptionsPtr()));
}

ImgDoc2ErrorCode DestroyCreateOptions(HandleCreateOptions handle, ImgDoc2ErrorInformation* error_information)
{
    const auto object = reinterpret_cast<PtrWrapper<ICreateOptions>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    if (!object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleCreateOptions", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    delete object;
    --g_imgdoc2_api_statistics.number_of_createoptions_objects_active;
    return ImgDoc2_ErrorCode_OK;
}

HandleOpenExistingOptions CreateOpenExistingOptions()
{
    ++g_imgdoc2_api_statistics.number_of_openexistingoptions_objects_active;
    return reinterpret_cast<HandleOpenExistingOptions>(new PtrWrapper(ClassFactory::CreateOpenExistingOptions()));
}

ImgDoc2ErrorCode DestroyOpenExistingOptions(HandleOpenExistingOptions handle, ImgDoc2ErrorInformation* error_information)
{
    const auto object = reinterpret_cast<PtrWrapper<IOpenExistingOptions>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    if (!object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleOpenExistingOptions", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    delete object;
    --g_imgdoc2_api_statistics.number_of_openexistingoptions_objects_active;
    return ImgDoc2_ErrorCode_OK;
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
        const auto hosting_environment_object = reinterpret_cast<SharedPtrWrapper<IHostingEnvironment>*>(handle_environment_object);  // NOLINT(performance-no-int-to-ptr)
        if (!hosting_environment_object->IsValid())
        {
            ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleEnvironmentObject", "The handle is invalid.", error_information);
            return ImgDoc2_ErrorCode_InvalidHandle;
        }

        hosting_environment = hosting_environment_object->shared_ptr_;
    }

    try
    {
        const auto create_options_object = reinterpret_cast<PtrWrapper<ICreateOptions>*>(create_options);  // NOLINT(performance-no-int-to-ptr)
        if (!create_options_object->IsValid())
        {
            ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleCreateOptions", "The handle is invalid.", error_information);
            return ImgDoc2_ErrorCode_InvalidHandle;
        }

        imgdoc2 = ClassFactory::CreateNew(create_options_object->ptr_, hosting_environment);  // NOLINT(performance-no-int-to-ptr)
    }
    catch (exception& exception)
    {
        ImgDoc2ApiSupport::FillOutErrorInformation(exception, error_information);
        return ImgDoc2ApiSupport::MapExceptionToReturnValue(exception);
    }

    auto shared_imgdoc_wrapping_object = new SharedPtrWrapper<IDoc>{ imgdoc2 };
    *document = reinterpret_cast<HandleDoc>(shared_imgdoc_wrapping_object);
    ++g_imgdoc2_api_statistics.number_of_document_objects_active;
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
        const auto hosting_environment_object = reinterpret_cast<SharedPtrWrapper<IHostingEnvironment>*>(handle_environment_object);  // NOLINT(performance-no-int-to-ptr)
        if (!hosting_environment_object->IsValid())
        {
            ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleEnvironmentObject", "The handle is invalid.", error_information);
            return ImgDoc2_ErrorCode_InvalidHandle;
        }

        hosting_environment = hosting_environment_object->shared_ptr_;
    }

    try
    {
        const auto open_existing_options_object = reinterpret_cast<PtrWrapper<IOpenExistingOptions>*>(open_existing_options);  // NOLINT(performance-no-int-to-ptr)
        if (!open_existing_options_object->IsValid())
        {
            ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleOpenExistingOptions", "The handle is invalid.", error_information);
            return ImgDoc2_ErrorCode_InvalidHandle;
        }

        imgdoc2 = ClassFactory::OpenExisting(open_existing_options_object->ptr_, hosting_environment);  // NOLINT(performance-no-int-to-ptr)
    }
    catch (exception& exception)
    {
        ImgDoc2ApiSupport::FillOutErrorInformation(exception, error_information);
        return ImgDoc2ApiSupport::MapExceptionToReturnValue(exception);
    }

    auto shared_imgdoc_wrapping_object = new SharedPtrWrapper<IDoc>{ imgdoc2 };
    *document = reinterpret_cast<HandleDoc>(shared_imgdoc_wrapping_object);
    ++g_imgdoc2_api_statistics.number_of_document_objects_active;
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode DestroyDocument(HandleDoc handle, ImgDoc2ErrorInformation* error_information)
{
    const auto object = reinterpret_cast<SharedPtrWrapper<IDoc>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    if (!object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDoc", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    delete object;
    --g_imgdoc2_api_statistics.number_of_document_objects_active;
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode IDoc_GetReader2d(HandleDoc handle_document, HandleDocRead2D* document_read2d, ImgDoc2ErrorInformation* error_information)
{
    if (document_read2d == nullptr)
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument("document_read2d", "must not be null", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    const auto document_object = reinterpret_cast<SharedPtrWrapper<IDoc>*>(handle_document);  // NOLINT(performance-no-int-to-ptr)
    if (!document_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDoc", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    auto reader2d = document_object->shared_ptr_->GetReader2d();
    if (reader2d)
    {
        auto shared_reader2d_wrapping_object = new SharedPtrWrapper<IDocRead2d>{ reader2d };
        *document_read2d = reinterpret_cast<HandleDocRead2D>(shared_reader2d_wrapping_object);
        ++g_imgdoc2_api_statistics.number_of_reader2d_objects_active;
    }
    else
    {
        *document_read2d = kInvalidObjectHandle;
    }

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode DestroyReader2d(HandleDocRead2D handle, ImgDoc2ErrorInformation* error_information)
{
    const auto object = reinterpret_cast<SharedPtrWrapper<IDocRead2d>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    if (!object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocRead2D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    delete object;
    --g_imgdoc2_api_statistics.number_of_reader2d_objects_active;
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode IDoc_GetReader3d(HandleDoc handle_document, HandleDocRead3D* document_read3d, ImgDoc2ErrorInformation* error_information)
{
    if (document_read3d == nullptr)
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument("document_read3d", "must not be null", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    const auto document_object = reinterpret_cast<SharedPtrWrapper<IDoc>*>(handle_document);  // NOLINT(performance-no-int-to-ptr)
    if (!document_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDoc", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    auto spReader3d = document_object->shared_ptr_->GetReader3d();
    //auto spReader3d = reinterpret_cast<SharedPtrWrapper<IDoc>*>(handle_document)->shared_ptr_->GetReader3d();   // NOLINT(performance-no-int-to-ptr)
    if (spReader3d)
    {
        auto shared_reader3d_wrapping_object = new SharedPtrWrapper<IDocRead3d>{ spReader3d };
        *document_read3d = reinterpret_cast<HandleDocRead3D>(shared_reader3d_wrapping_object);
        ++g_imgdoc2_api_statistics.number_of_reader3d_objects_active;
    }
    else
    {
        *document_read3d = kInvalidObjectHandle;
    }

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode DestroyReader3d(HandleDocRead3D handle, ImgDoc2ErrorInformation* error_information)
{
    const auto object = reinterpret_cast<SharedPtrWrapper<IDocRead3d>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    if (!object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocRead3D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    delete object;
    --g_imgdoc2_api_statistics.number_of_reader3d_objects_active;
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode IDoc_GetWriter2d(HandleDoc handle_document, HandleDocWrite2D* document_writer2d, ImgDoc2ErrorInformation* error_information)
{
    if (document_writer2d == nullptr)
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument("document_writer2d", "must not be null", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    const auto document_object = reinterpret_cast<SharedPtrWrapper<IDoc>*>(handle_document);  // NOLINT(performance-no-int-to-ptr)
    if (!document_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDoc", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    auto writer2d = document_object->shared_ptr_->GetWriter2d();
    if (writer2d)
    {
        auto shared_writer2d_wrapping_object = new SharedPtrWrapper<IDocWrite2d>{ writer2d };
        *document_writer2d = reinterpret_cast<HandleDocWrite2D>(shared_writer2d_wrapping_object);
        ++g_imgdoc2_api_statistics.number_of_writer2d_objects_active;
    }
    else
    {
        *document_writer2d = kInvalidObjectHandle;
    }

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode DestroyWriter2d(HandleDocWrite2D handle, ImgDoc2ErrorInformation* error_information)
{
    const auto object = reinterpret_cast<SharedPtrWrapper<IDocWrite2d>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    if (!object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocWrite2D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    delete object;
    --g_imgdoc2_api_statistics.number_of_writer2d_objects_active;
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode IDoc_GetWriter3d(HandleDoc handle_document, HandleDocWrite3D* document_writer3d, ImgDoc2ErrorInformation* error_information)
{
    if (document_writer3d == nullptr)
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument("document_writer3d", "must not be null", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    const auto document_object = reinterpret_cast<SharedPtrWrapper<IDoc>*>(handle_document);  // NOLINT(performance-no-int-to-ptr)
    if (!document_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDoc", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    auto writer3d = document_object->shared_ptr_->GetWriter3d();
    if (writer3d)
    {
        auto shared_writer3d_wrapping_object = new SharedPtrWrapper<IDocWrite3d>{ writer3d };
        *document_writer3d = reinterpret_cast<HandleDocWrite2D>(shared_writer3d_wrapping_object);
        ++g_imgdoc2_api_statistics.number_of_writer3d_objects_active;
    }
    else
    {
        *document_writer3d = kInvalidObjectHandle;
    }

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode DestroyWriter3d(HandleDocWrite2D handle, ImgDoc2ErrorInformation* error_information)
{
    const auto object = reinterpret_cast<SharedPtrWrapper<IDocWrite3d>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    if (!object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocWrite2D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    delete object;
    --g_imgdoc2_api_statistics.number_of_writer3d_objects_active;
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_SetFilename(HandleCreateOptions handle, const char* filename_utf8, ImgDoc2ErrorInformation* error_information)
{
    const auto create_options_object = reinterpret_cast<PtrWrapper<ICreateOptions>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    if (!create_options_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleCreateOptions", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    create_options_object->ptr_->SetFilename(filename_utf8);
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode OpenExistingOptions_SetFilename(HandleOpenExistingOptions handle, const char* filename_utf8, ImgDoc2ErrorInformation* error_information)
{
    const auto open_existing_options_object = reinterpret_cast<PtrWrapper<IOpenExistingOptions>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    if (!open_existing_options_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleOpenExistingOptions", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    open_existing_options_object->ptr_->SetFilename(filename_utf8);
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_SetDocumentType(HandleCreateOptions handle, std::uint8_t document_type_interop, ImgDoc2ErrorInformation* error_information)
{
    const auto create_options_object = reinterpret_cast<PtrWrapper<ICreateOptions>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    if (!create_options_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleCreateOptions", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto document_type = Utilities::ConvertDocumentTypeFromInterop(document_type_interop);
    try
    {
        create_options_object->ptr_->SetDocumentType(document_type);
    }
    catch (const std::exception& exception)
    {
        ImgDoc2ApiSupport::FillOutErrorInformation(exception, error_information);
        return ImgDoc2ApiSupport::MapExceptionToReturnValue(exception);
    }

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_SetUseSpatialIndex(HandleCreateOptions handle, bool use_spatial_index, ImgDoc2ErrorInformation* error_information)
{
    const auto create_options_object = reinterpret_cast<PtrWrapper<ICreateOptions>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    if (!create_options_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleCreateOptions", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    create_options_object->ptr_->SetUseSpatialIndex(use_spatial_index);
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_SetUseBlobTable(HandleCreateOptions handle, bool use_blob_table, ImgDoc2ErrorInformation* error_information)
{
    const auto create_options_object = reinterpret_cast<PtrWrapper<ICreateOptions>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    if (!create_options_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleCreateOptions", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    create_options_object->ptr_->SetCreateBlobTable(use_blob_table);
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_AddIndexForDimension(HandleCreateOptions handle, char dimension, ImgDoc2ErrorInformation* error_information)
{
    const auto create_options_object = reinterpret_cast<PtrWrapper<ICreateOptions>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    if (!create_options_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleCreateOptions", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    create_options_object->ptr_->AddIndexForDimension(dimension);
    return ImgDoc2_ErrorCode_OK;
}

template <typename tGetString>
static ImgDoc2ErrorCode ReturnStringHelper(const tGetString& getString, char* filename_utf8, size_t* size, ImgDoc2ErrorInformation* error_information)
{
    if (size == nullptr || *size < 1)
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument("size", "must not be null and greater than zero", error_information);
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
    const auto create_options_object = reinterpret_cast<PtrWrapper<ICreateOptions>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    if (!create_options_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleCreateOptions", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    return ReturnStringHelper(
        [=]()->std::string
        {
            return create_options_object->ptr_->GetFilename();
        },
        filename_utf8,
            size,
            error_information);
}

ImgDoc2ErrorCode OpenExistingOptions_GetFilename(HandleOpenExistingOptions handle, char* filename_utf8, size_t* size, ImgDoc2ErrorInformation* error_information)
{
    const auto open_existing_options_object = reinterpret_cast<PtrWrapper<IOpenExistingOptions>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    if (!open_existing_options_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleOpenExistingOptions", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    return ReturnStringHelper(
        [=]()->std::string
        {
            return open_existing_options_object->ptr_->GetFilename();
        },
        filename_utf8,
            size,
            error_information);
}

ImgDoc2ErrorCode CreateOptions_GetDocumentType(HandleCreateOptions handle, std::uint8_t* document_type_interop, ImgDoc2ErrorInformation* error_information)
{
    if (document_type_interop == nullptr)
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument("document_type_interop", "must not be null", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    const auto create_options_object = reinterpret_cast<PtrWrapper<ICreateOptions>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    if (!create_options_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleCreateOptions", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto document_type = create_options_object->ptr_->GetDocumentType();
    *document_type_interop = static_cast<uint8_t>(document_type);
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_GetUseSpatialIndex(HandleCreateOptions handle, bool* use_spatial_index, ImgDoc2ErrorInformation* error_information)
{
    const auto create_options_object = reinterpret_cast<PtrWrapper<ICreateOptions>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    if (!create_options_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleCreateOptions", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const bool b = create_options_object->ptr_->GetUseSpatialIndex();
    if (use_spatial_index != nullptr)
    {
        *use_spatial_index = b;
    }

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_GetUseBlobTable(HandleCreateOptions handle, bool* use_blob_table, ImgDoc2ErrorInformation* error_information)
{
    const auto create_options_object = reinterpret_cast<PtrWrapper<ICreateOptions>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    if (!create_options_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleCreateOptions", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const bool b = create_options_object->ptr_->GetCreateBlobTable();
    if (use_blob_table != nullptr)
    {
        *use_blob_table = b;
    }

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_AddDimension(HandleCreateOptions handle, std::uint8_t dimension, ImgDoc2ErrorInformation* error_information)
{
    const auto create_options_object = reinterpret_cast<PtrWrapper<ICreateOptions>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    if (!create_options_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleCreateOptions", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    try
    {
        create_options_object->ptr_->AddDimension(gsl::narrow_cast<Dimension>(dimension));
    }
    catch (exception& exception)
    {
        ImgDoc2ApiSupport::FillOutErrorInformation(exception, error_information);
        return ImgDoc2ApiSupport::MapExceptionToReturnValue(exception);
    }

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_AddIndexedDimension(HandleCreateOptions handle, std::uint8_t dimension, ImgDoc2ErrorInformation* error_information)
{
    const auto create_options_object = reinterpret_cast<PtrWrapper<ICreateOptions>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    if (!create_options_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleCreateOptions", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    try
    {
        create_options_object->ptr_->AddIndexForDimension(gsl::narrow_cast<Dimension>(dimension));
    }
    catch (exception& exception)
    {
        ImgDoc2ApiSupport::FillOutErrorInformation(exception, error_information);
        return ImgDoc2ApiSupport::MapExceptionToReturnValue(exception);
    }

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_GetDimensions(HandleCreateOptions handle, std::uint8_t* dimensions, size_t* elements_count, ImgDoc2ErrorInformation* error_information)
{
    if (elements_count == nullptr)
    {
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    const auto create_options_object = reinterpret_cast<PtrWrapper<ICreateOptions>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    if (!create_options_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleCreateOptions", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto dimensions_from_object = create_options_object->ptr_->GetDimensions();

    size_t count = 0;
    for (const auto d : dimensions_from_object)
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
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument("elements_count", "must not be null", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    const auto create_options_object = reinterpret_cast<PtrWrapper<ICreateOptions>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    if (!create_options_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleCreateOptions", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto dimensions_from_object = create_options_object->ptr_->GetIndexedDimensions();

    size_t count = 0;
    for (const auto d : dimensions_from_object)
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
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument("tile_coordinate_interop", "must not be null", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    if (logical_position_info_interop == nullptr)
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument("logical_position_info_interop", "must not be null", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    const auto tile_coordinate = Utilities::ConvertToTileCoordinate(tile_coordinate_interop);
    const auto logical_position_info = Utilities::ConvertLogicalPositionInfoInteropToImgdoc2(*logical_position_info_interop);
    const TileBaseInfo tile_info = Utilities::ConvertTileBaseInfoInteropToImgdoc2(*tile_base_info_interop);
    const DataTypes data_type = Utilities::ConvertDatatypeEnumInterop(data_type_interop);

    const auto write2d_object = reinterpret_cast<SharedPtrWrapper<IDocWrite2d>*>(handle); // NOLINT(performance-no-int-to-ptr)
    if (!write2d_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocWrite2D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto writer2d = write2d_object->shared_ptr_;

    try
    {
        const Utilities::GetDataObject data_object(ptr_data, size_data);
        const auto pk = writer2d->AddTile(
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
        ImgDoc2ApiSupport::FillOutErrorInformation(exception, error_information);
        return ImgDoc2ApiSupport::MapExceptionToReturnValue(exception);
    }

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode IDocWrite3d_AddBrick(
    HandleDocWrite3D handle,
    const TileCoordinateInterop* tile_coordinate_interop,
    const LogicalPositionInfo3DInterop* logical_position_info_interop,
    const BrickBaseInfoInterop* brick_base_info_interop,
    std::uint8_t data_type_interop,
    const void* ptr_data,
    std::uint64_t size_data,
    imgdoc2::dbIndex* result_pk,
    ImgDoc2ErrorInformation* error_information)
{
    if (tile_coordinate_interop == nullptr)
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument("tile_coordinate_interop", "must not be null", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    if (logical_position_info_interop == nullptr)
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument("logical_position_info_interop", "must not be null", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    const auto tile_coordinate = Utilities::ConvertToTileCoordinate(tile_coordinate_interop);
    const auto logical_position_info = Utilities::ConvertLogicalPositionInfo3DInteropToImgdoc2(*logical_position_info_interop);
    const BrickBaseInfo tile_info = Utilities::ConvertBrickBaseInfoInteropToImgdoc2(*brick_base_info_interop);
    const DataTypes data_type = Utilities::ConvertDatatypeEnumInterop(data_type_interop);

    const auto write3d_object = reinterpret_cast<SharedPtrWrapper<IDocWrite3d>*>(handle); // NOLINT(performance-no-int-to-ptr)
    if (!write3d_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocWrite3D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto writer3d = write3d_object->shared_ptr_;

    try
    {
        const Utilities::GetDataObject data_object(ptr_data, size_data);
        const auto pk = writer3d->AddBrick(
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
        ImgDoc2ApiSupport::FillOutErrorInformation(exception, error_information);
        return ImgDoc2ApiSupport::MapExceptionToReturnValue(exception);
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
    const auto reader2d_object = reinterpret_cast<SharedPtrWrapper<IDocRead2d>*>(handle); // NOLINT(performance-no-int-to-ptr)
    if (!reader2d_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocRead2D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto reader2d = reader2d_object->shared_ptr_;

    const auto dimension_coordinate_query_clause = dim_coordinate_query_clause_interop != nullptr ?
        Utilities::ConvertDimensionQueryRangeClauseInteropToImgdoc2(dim_coordinate_query_clause_interop) :
        CDimCoordinateQueryClause();
    const auto tile_info_query_clause = tile_info_query_clause_interop != nullptr ?
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
        ImgDoc2ApiSupport::FillOutErrorInformation(exception, error_information);
        return ImgDoc2ApiSupport::MapExceptionToReturnValue(exception);
    }

    result->element_count = results_retrieved_count;

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode IDocRead3d_Query(
    HandleDocRead3D handle,
    const DimensionQueryClauseInterop* dim_coordinate_query_clause_interop,
    const TileInfoQueryClauseInterop* tile_info_query_clause_interop,
    QueryResultInterop* result,
    ImgDoc2ErrorInformation* error_information)
{
    const auto reader3d_object = reinterpret_cast<SharedPtrWrapper<IDocRead3d>*>(handle); // NOLINT(performance-no-int-to-ptr)
    if (!reader3d_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocRead3D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto reader3d = reader3d_object->shared_ptr_;

    const auto dimension_coordinate_query_clause = dim_coordinate_query_clause_interop != nullptr ?
        Utilities::ConvertDimensionQueryRangeClauseInteropToImgdoc2(dim_coordinate_query_clause_interop) :
        CDimCoordinateQueryClause();
    const auto tile_info_query_clause = tile_info_query_clause_interop != nullptr ?
        Utilities::ConvertTileInfoQueryClauseInteropToImgdoc2(tile_info_query_clause_interop) :
        CTileInfoQueryClause();

    uint32_t results_retrieved_count = 0;
    result->more_results_available = 0;
    try
    {
        reader3d->Query(
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
        ImgDoc2ApiSupport::FillOutErrorInformation(exception, error_information);
        return ImgDoc2ApiSupport::MapExceptionToReturnValue(exception);
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
    const auto reader2d_object = reinterpret_cast<SharedPtrWrapper<IDocRead2d>*>(handle); // NOLINT(performance-no-int-to-ptr)
    if (!reader2d_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocRead2D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto reader2d = reader2d_object->shared_ptr_;

    const auto tile_info_query_clause = tile_info_query_clause_interop != nullptr ?
        Utilities::ConvertTileInfoQueryClauseInteropToImgdoc2(tile_info_query_clause_interop) :
        CTileInfoQueryClause();
    const auto dimension_coordinate_query_clause = dim_coordinate_query_clause_interop != nullptr ?
        Utilities::ConvertDimensionQueryRangeClauseInteropToImgdoc2(dim_coordinate_query_clause_interop) :
        CDimCoordinateQueryClause();

    const RectangleD rectangle = Utilities::ConvertRectangleDoubleInterop(*query_rectangle);
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
        ImgDoc2ApiSupport::FillOutErrorInformation(exception, error_information);
        return ImgDoc2ApiSupport::MapExceptionToReturnValue(exception);
    }

    result->element_count = results_retrieved_count;

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode IDocRead3d_GetBricksIntersectingCuboid(
    HandleDocRead3D handle,
    const CuboidDoubleInterop* query_cuboid,
    const DimensionQueryClauseInterop* dim_coordinate_query_clause_interop,
    const TileInfoQueryClauseInterop* tile_info_query_clause_interop,
    QueryResultInterop* result,
    ImgDoc2ErrorInformation* error_information)
{
    const auto reader3d_object = reinterpret_cast<SharedPtrWrapper<IDocRead3d>*>(handle); // NOLINT(performance-no-int-to-ptr)
    if (!reader3d_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocRead3D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto reader3d = reader3d_object->shared_ptr_;

    const auto tile_info_query_clause = tile_info_query_clause_interop != nullptr ?
        Utilities::ConvertTileInfoQueryClauseInteropToImgdoc2(tile_info_query_clause_interop) :
        CTileInfoQueryClause();
    const auto dimension_coordinate_query_clause = dim_coordinate_query_clause_interop != nullptr ?
        Utilities::ConvertDimensionQueryRangeClauseInteropToImgdoc2(dim_coordinate_query_clause_interop) :
        CDimCoordinateQueryClause();

    const CuboidD cuboid = Utilities::ConvertCuboidDoubleInterop(*query_cuboid);
    uint32_t results_retrieved_count = 0;
    result->more_results_available = 0;
    try
    {
        reader3d->GetTilesIntersectingCuboid(
            cuboid,
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
        ImgDoc2ApiSupport::FillOutErrorInformation(exception, error_information);
        return ImgDoc2ApiSupport::MapExceptionToReturnValue(exception);
    }

    result->element_count = results_retrieved_count;
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode IDocRead3d_GetBricksIntersectingPlane(
    HandleDocRead3D handle,
    const PlaneNormalAndDistanceInterop* plane_normal_and_distance_interop,
    const DimensionQueryClauseInterop* dim_coordinate_query_clause_interop,
    const TileInfoQueryClauseInterop* tile_info_query_clause_interop,
    QueryResultInterop* result,
    ImgDoc2ErrorInformation* error_information)
{
    const auto reader3d_object = reinterpret_cast<SharedPtrWrapper<IDocRead3d>*>(handle); // NOLINT(performance-no-int-to-ptr)
    if (!reader3d_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocRead3D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto reader3d = reader3d_object->shared_ptr_;

    const auto tile_info_query_clause = tile_info_query_clause_interop != nullptr ?
        Utilities::ConvertTileInfoQueryClauseInteropToImgdoc2(tile_info_query_clause_interop) :
        CTileInfoQueryClause();
    const auto dimension_coordinate_query_clause = dim_coordinate_query_clause_interop != nullptr ?
        Utilities::ConvertDimensionQueryRangeClauseInteropToImgdoc2(dim_coordinate_query_clause_interop) :
        CDimCoordinateQueryClause();
    const auto plane_normal_and_distance = Utilities::ConvertPlaneNormalAndDistanceInterop(*plane_normal_and_distance_interop);
    uint32_t results_retrieved_count = 0;
    result->more_results_available = 0;
    try
    {
        reader3d->GetTilesIntersectingPlane(
            plane_normal_and_distance,
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
        ImgDoc2ApiSupport::FillOutErrorInformation(exception, error_information);
        return ImgDoc2ApiSupport::MapExceptionToReturnValue(exception);
    }

    result->element_count = results_retrieved_count;
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode IDocRead2d_ReadTileData(
    HandleDocRead2D handle,
    std::int64_t pk,
    std::intptr_t blob_output_handle,
    MemTransferReserveFunctionPointer pfnReserve,
    MemTransferSetDataFunctionPointer pfnSetData,
    ImgDoc2ErrorInformation* error_information)
{
    static_assert(sizeof(pk) == sizeof(imgdoc2::dbIndex), "Type of the argument 'pk' and the imgdoc2-dbIndex-type must have same size.");

    const auto reader2d_object = reinterpret_cast<SharedPtrWrapper<IDocRead2d>*>(handle); // NOLINT(performance-no-int-to-ptr)
    if (!reader2d_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocRead2D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto reader2d = reader2d_object->shared_ptr_;

    Utilities::BlobOutputOnFunctionsDecorator blob_output_object(blob_output_handle, pfnReserve, pfnSetData);
    try
    {
        reader2d->ReadTileData(pk, &blob_output_object);
    }
    catch (exception& exception)
    {
        ImgDoc2ApiSupport::FillOutErrorInformation(exception, error_information);
        return ImgDoc2ApiSupport::MapExceptionToReturnValue(exception);
    }

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode IDocRead3d_ReadBrickData(
    HandleDocRead3D handle,
    std::int64_t pk,
    std::intptr_t blob_output_handle,
    MemTransferReserveFunctionPointer pfnReserve,
    MemTransferSetDataFunctionPointer pfnSetData,
    ImgDoc2ErrorInformation* error_information)
{
    static_assert(sizeof(pk) == sizeof(imgdoc2::dbIndex), "Type of the argument 'pk' and the imgdoc2-dbIndex-type must have same size.");

    const auto reader3d_object = reinterpret_cast<SharedPtrWrapper<IDocRead3d>*>(handle); // NOLINT(performance-no-int-to-ptr)
    if (!reader3d_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocRead3D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto reader3d = reader3d_object->shared_ptr_;

    Utilities::BlobOutputOnFunctionsDecorator blob_output_object(blob_output_handle, pfnReserve, pfnSetData);
    try
    {
        reader3d->ReadBrickData(pk, &blob_output_object);
    }
    catch (exception& exception)
    {
        ImgDoc2ApiSupport::FillOutErrorInformation(exception, error_information);
        return ImgDoc2ApiSupport::MapExceptionToReturnValue(exception);
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

    const auto reader2d_object = reinterpret_cast<SharedPtrWrapper<IDocRead2d>*>(handle); // NOLINT(performance-no-int-to-ptr)
    if (!reader2d_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocRead2D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto reader2d = reader2d_object->shared_ptr_;

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
        ImgDoc2ApiSupport::FillOutErrorInformation(exception, error_information);
        return ImgDoc2ApiSupport::MapExceptionToReturnValue(exception);
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

ImgDoc2ErrorCode IDocRead3d_ReadBrickInfo(
    HandleDocRead3D handle,
    std::int64_t pk,
    TileCoordinateInterop* tile_coordinate_interop,
    LogicalPositionInfo3DInterop* logical_position_info3d_interop,
    BrickBlobInfoInterop* brick_blob_info_interop,
    ImgDoc2ErrorInformation* error_information)
{
    static_assert(sizeof(pk) == sizeof(imgdoc2::dbIndex), "Type of the argument 'pk' and the imgdoc2-dbIndex-type must have same size.");

    const auto reader3d_object = reinterpret_cast<SharedPtrWrapper<IDocRead3d>*>(handle); // NOLINT(performance-no-int-to-ptr)
    if (!reader3d_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocRead3D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto reader3d = reader3d_object->shared_ptr_;

    LogicalPositionInfo3D logical_position_info3d;
    TileCoordinate tile_coordinate;
    BrickBlobInfo brick_blob_info;
    try
    {
        reader3d->ReadBrickInfo(
            pk,
            tile_coordinate_interop != nullptr ? &tile_coordinate : nullptr,
            logical_position_info3d_interop != nullptr ? &logical_position_info3d : nullptr,
            brick_blob_info_interop != nullptr ? &brick_blob_info : nullptr);
    }
    catch (exception& exception)
    {
        ImgDoc2ApiSupport::FillOutErrorInformation(exception, error_information);
        return ImgDoc2ApiSupport::MapExceptionToReturnValue(exception);
    }

    if (tile_coordinate_interop != nullptr)
    {
        const bool b = Utilities::TryConvertToTileCoordinateInterop(&tile_coordinate, tile_coordinate_interop);
        if (!b)
        {
            // TODO(JBL): implement error-handling
        }
    }

    if (logical_position_info3d_interop != nullptr)
    {
        *logical_position_info3d_interop = Utilities::ConvertImgDoc2LogicalPositionInfo3DToInterop(logical_position_info3d);
    }

    if (brick_blob_info_interop != nullptr)
    {
        *brick_blob_info_interop = Utilities::ConvertImgDoc2BrickBlobInfoToInterop(brick_blob_info);
    }

    return ImgDoc2_ErrorCode_OK;
}

// *********** IDocInfo2d_GetTileDimensions/IDocInfo3d_GetTileDimensions ***********
static ImgDoc2ErrorCode IDocInfo_GetTileDimensions(
    imgdoc2::IDocInfo* doc_info,
    imgdoc2::Dimension* dimensions,
    std::uint32_t* count,
    ImgDoc2ErrorInformation* error_information)
{
    if (count == nullptr)
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument("count", "must not be null", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    if (*count > 0 && dimensions == nullptr)
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument("dimensions", "must not be null (if a count > 0 was given)", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    doc_info->GetTileDimensions(dimensions, *count);
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode IDocInfo2d_GetTileDimensions(
    HandleDocRead2D handle,
    imgdoc2::Dimension* dimensions,
    std::uint32_t* count,
    ImgDoc2ErrorInformation* error_information)
{
    const auto reader2d_object = reinterpret_cast<SharedPtrWrapper<IDocRead2d>*>(handle); // NOLINT(performance-no-int-to-ptr)
    if (!reader2d_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocRead2D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto reader2d = reader2d_object->shared_ptr_;
    return IDocInfo_GetTileDimensions(reader2d.get(), dimensions, count, error_information);
}

ImgDoc2ErrorCode IDocInfo3d_GetTileDimensions(
    HandleDocRead3D handle,
    imgdoc2::Dimension* dimensions,
    std::uint32_t* count,
    ImgDoc2ErrorInformation* error_information)
{
    const auto reader3d_object = reinterpret_cast<SharedPtrWrapper<IDocRead3d>*>(handle); // NOLINT(performance-no-int-to-ptr)
    if (!reader3d_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocRead3D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto reader3d = reader3d_object->shared_ptr_;
    return IDocInfo_GetTileDimensions(reader3d.get(), dimensions, count, error_information);
}

// *********** IDocInfo2d_GetMinMaxForTileDimensions/IDocInfo3d_GetMinMaxForTileDimensions ***********
static ImgDoc2ErrorCode IDocInfo_GetMinMaxForTileDimensions(
    imgdoc2::IDocInfo* doc_info,
    const imgdoc2::Dimension* dimensions,
    std::uint32_t count,
    MinMaxForTilePositionsInterop* result,
    ImgDoc2ErrorInformation* error_information)
{
    if (dimensions == nullptr)
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument("dimensions", "must not be null", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    if (result == nullptr)
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument("result", "must not be null", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    const vector<Dimension> dimensions_array(dimensions, dimensions + count);
    std::map<imgdoc2::Dimension, imgdoc2::Int32Interval> min_max;

    try
    {
        min_max = doc_info->GetMinMaxForTileDimension(dimensions_array);
    }
    catch (exception& exception)
    {
        ImgDoc2ApiSupport::FillOutErrorInformation(exception, error_information);
        return ImgDoc2ApiSupport::MapExceptionToReturnValue(exception);
    }

    for (uint32_t i = 0; i < count; ++i)
    {
        const auto& item = min_max.at(dimensions[i]);
        (result + i)->minimum_value = item.minimum_value;
        (result + i)->maximum_value = item.maximum_value;
    }

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode IDocInfo2d_GetMinMaxForTileDimensions(
    HandleDocRead2D handle,
    const imgdoc2::Dimension* dimensions,
    std::uint32_t count,
    MinMaxForTilePositionsInterop* result,
    ImgDoc2ErrorInformation* error_information)
{
    const auto reader2d_object = reinterpret_cast<SharedPtrWrapper<IDocRead2d>*>(handle); // NOLINT(performance-no-int-to-ptr)
    if (!reader2d_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocRead2D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto reader2d = reader2d_object->shared_ptr_;
    return IDocInfo_GetMinMaxForTileDimensions(reader2d.get(), dimensions, count, result, error_information);
}

ImgDoc2ErrorCode IDocInfo3d_GetMinMaxForTileDimensions(
    HandleDocRead3D handle,
    const imgdoc2::Dimension* dimensions,
    std::uint32_t count,
    MinMaxForTilePositionsInterop* result,
    ImgDoc2ErrorInformation* error_information)
{
    const auto reader3d_object = reinterpret_cast<SharedPtrWrapper<IDocRead3d>*>(handle); // NOLINT(performance-no-int-to-ptr)
    if (!reader3d_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocRead3D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto reader3d = reader3d_object->shared_ptr_;
    return IDocInfo_GetMinMaxForTileDimensions(reader3d.get(), dimensions, count, result, error_information);
}

ImgDoc2ErrorCode IDocInfo2d_GetBoundingBoxForTiles(
    HandleDocRead2D handle,
    double* min_x,
    double* max_x,
    double* min_y,
    double* max_y,
    ImgDoc2ErrorInformation* error_information)
{
    const auto reader2d_object = reinterpret_cast<SharedPtrWrapper<IDocRead2d>*>(handle); // NOLINT(performance-no-int-to-ptr)
    if (!reader2d_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocRead2D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto reader2d = reader2d_object->shared_ptr_;

    DoubleInterval interval_x, interval_y;
    DoubleInterval* pointer_interval_x{ nullptr }, * pointer_interval_y{ nullptr };
    if (min_x != nullptr || max_x != nullptr)
    {
        pointer_interval_x = &interval_x;
    }

    if (min_y != nullptr || max_y != nullptr)
    {
        pointer_interval_y = &interval_y;
    }

    try
    {
        reader2d->GetTilesBoundingBox(pointer_interval_x, pointer_interval_y);
    }
    catch (exception& exception)
    {
        ImgDoc2ApiSupport::FillOutErrorInformation(exception, error_information);
        return ImgDoc2ApiSupport::MapExceptionToReturnValue(exception);
    }

    if (min_x != nullptr)
    {
        *min_x = interval_x.IsValid() ? interval_x.minimum_value : std::numeric_limits<double>::max();
    }

    if (max_x != nullptr)
    {
        *max_x = interval_x.IsValid() ? interval_x.maximum_value : std::numeric_limits<double>::lowest();
    }

    if (min_y != nullptr)
    {
        *min_y = interval_y.IsValid() ? interval_y.minimum_value : std::numeric_limits<double>::max();
    }

    if (max_y != nullptr)
    {
        *max_y = interval_y.IsValid() ? interval_y.maximum_value : std::numeric_limits<double>::lowest();
    }

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode IDocInfo3d_GetBoundingBoxForBricks(
    HandleDocRead3D handle,
    double* min_x,
    double* max_x,
    double* min_y,
    double* max_y,
    double* min_z,
    double* max_z,
    ImgDoc2ErrorInformation* error_information)
{
    const auto reader3d_object = reinterpret_cast<SharedPtrWrapper<IDocRead3d>*>(handle); // NOLINT(performance-no-int-to-ptr)
    if (!reader3d_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocRead3D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto reader3d = reader3d_object->shared_ptr_;

    DoubleInterval interval_x, interval_y, interval_z;
    DoubleInterval* pointer_interval_x{ nullptr }, * pointer_interval_y{ nullptr }, * pointer_interval_z{ nullptr };
    if (min_x != nullptr || max_x != nullptr)
    {
        pointer_interval_x = &interval_x;
    }

    if (min_y != nullptr || max_y != nullptr)
    {
        pointer_interval_y = &interval_y;
    }

    if (min_z != nullptr || max_z != nullptr)
    {
        pointer_interval_z = &interval_z;
    }

    try
    {
        reader3d->GetBricksBoundingBox(pointer_interval_x, pointer_interval_y, pointer_interval_z);
    }
    catch (exception& exception)
    {
        ImgDoc2ApiSupport::FillOutErrorInformation(exception, error_information);
        return ImgDoc2ApiSupport::MapExceptionToReturnValue(exception);
    }

    if (min_x != nullptr)
    {
        *min_x = interval_x.IsValid() ? interval_x.minimum_value : std::numeric_limits<double>::max();
    }

    if (max_x != nullptr)
    {
        *max_x = interval_x.IsValid() ? interval_x.maximum_value : std::numeric_limits<double>::lowest();
    }

    if (min_y != nullptr)
    {
        *min_y = interval_y.IsValid() ? interval_y.minimum_value : std::numeric_limits<double>::max();
    }

    if (max_y != nullptr)
    {
        *max_y = interval_y.IsValid() ? interval_y.maximum_value : std::numeric_limits<double>::lowest();
    }

    if (min_z != nullptr)
    {
        *min_z = interval_z.IsValid() ? interval_z.minimum_value : std::numeric_limits<double>::max();
    }

    if (max_z != nullptr)
    {
        *max_z = interval_z.IsValid() ? interval_z.maximum_value : std::numeric_limits<double>::lowest();
    }

    return ImgDoc2_ErrorCode_OK;
}

// *********** IDocInfo2d_GetTotalTileCount/IDocInfo3d_GetTotalTileCount ***********
static ImgDoc2ErrorCode IDocInfo_GetTotalTileCount(
        imgdoc2::IDocInfo* doc_info,
        std::uint64_t* total_tile_count,
        ImgDoc2ErrorInformation* error_information)
{
    if (total_tile_count == nullptr)
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument("total_tile_count", "must not be null", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    try
    {
        *total_tile_count = doc_info->GetTotalTileCount();
    }
    catch (exception& exception)
    {
        ImgDoc2ApiSupport::FillOutErrorInformation(exception, error_information);
        return ImgDoc2ApiSupport::MapExceptionToReturnValue(exception);
    }

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode IDocInfo2d_GetTotalTileCount(
        HandleDocRead2D handle,
        std::uint64_t* total_tile_count,
        ImgDoc2ErrorInformation* error_information)
{
    const auto reader2d_object = reinterpret_cast<SharedPtrWrapper<IDocRead2d>*>(handle); // NOLINT(performance-no-int-to-ptr)
    if (!reader2d_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocRead2D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto reader2d = reader2d_object->shared_ptr_;
    return IDocInfo_GetTotalTileCount(reader2d.get(), total_tile_count, error_information);
}

ImgDoc2ErrorCode IDocInfo3d_GetTotalTileCount(
        HandleDocRead2D handle,
        std::uint64_t* total_tile_count,
        ImgDoc2ErrorInformation* error_information)
{
    const auto reader3d_object = reinterpret_cast<SharedPtrWrapper<IDocRead3d>*>(handle); // NOLINT(performance-no-int-to-ptr)
    if (!reader3d_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocRead3D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto reader3d = reader3d_object->shared_ptr_;
    return IDocInfo_GetTotalTileCount(reader3d.get(), total_tile_count, error_information);
}

// *********** IDocInfo2d_GetTileCountPerLayer/IDocInfo3d_GetTileCountPerLayer ***********
static ImgDoc2ErrorCode IDocInfo_GetTileCountPerLayer(
        imgdoc2::IDocInfo* doc_info,
        TileCountPerLayerInterop* tile_count_per_layer_interop,
        ImgDoc2ErrorInformation* error_information)
{
    if (tile_count_per_layer_interop == nullptr)
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument("tile_count_per_layer_interop", "must not be null", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    try
    {
        const auto tile_count_per_layer = doc_info->GetTileCountPerLayer();
        tile_count_per_layer_interop->element_count_available = 0;
        for (const auto& item : tile_count_per_layer)
        {
            if (tile_count_per_layer_interop->element_count_available < tile_count_per_layer_interop->element_count_allocated)
            {
                PerLayerTileCountInterop* per_layer_tile_count_interop = tile_count_per_layer_interop->pyramid_layer_and_tile_count + tile_count_per_layer_interop->element_count_available;
                per_layer_tile_count_interop->layer_index = item.first;
                per_layer_tile_count_interop->tile_count = item.second;
            }

            ++tile_count_per_layer_interop->element_count_available;
        }
    }
    catch (exception& exception)
    {
        ImgDoc2ApiSupport::FillOutErrorInformation(exception, error_information);
        return ImgDoc2ApiSupport::MapExceptionToReturnValue(exception);
    }

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode IDocInfo2d_GetTileCountPerLayer(
        HandleDocRead2D handle,
        TileCountPerLayerInterop* tile_count_per_layer_interop,
        ImgDoc2ErrorInformation* error_information)
{
    const auto reader2d_object = reinterpret_cast<SharedPtrWrapper<IDocRead2d>*>(handle); // NOLINT(performance-no-int-to-ptr)
    if (!reader2d_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocRead2D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto reader2d = reader2d_object->shared_ptr_;
    return IDocInfo_GetTileCountPerLayer(reader2d.get(), tile_count_per_layer_interop, error_information);
}

ImgDoc2ErrorCode IDocInfo3d_GetTileCountPerLayer(
        HandleDocRead3D handle,
        TileCountPerLayerInterop* tile_count_per_layer_interop,
        ImgDoc2ErrorInformation* error_information)
{
    const auto reader3d_object = reinterpret_cast<SharedPtrWrapper<IDocRead3d>*>(handle); // NOLINT(performance-no-int-to-ptr)
    if (!reader3d_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocRead3D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto reader3d = reader3d_object->shared_ptr_;
    return IDocInfo_GetTileCountPerLayer(reader3d.get(), tile_count_per_layer_interop, error_information);
}

static ImgDoc2ErrorCode IDocWriter2d_TransactionCommon(HandleDocWrite2D handle, ImgDoc2ErrorInformation* error_information, void(IDatabaseTransaction::* mfp)())
{
    const auto writer2d_object = reinterpret_cast<SharedPtrWrapper<IDocWrite2d>*>(handle); // NOLINT(performance-no-int-to-ptr)
    if (!writer2d_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocWrite2D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto writer2d = writer2d_object->shared_ptr_;
    try
    {
        (writer2d.get()->*mfp)();
    }
    catch (exception& exception)
    {
        ImgDoc2ApiSupport::FillOutErrorInformation(exception, error_information);
        return ImgDoc2ApiSupport::MapExceptionToReturnValue(exception);
    }

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode IDocWrite2d_BeginTransaction(HandleDocWrite2D handle, ImgDoc2ErrorInformation* error_information)
{
    return IDocWriter2d_TransactionCommon(handle, error_information, &IDatabaseTransaction::BeginTransaction);
}

ImgDoc2ErrorCode IDocWrite2d_CommitTransaction(HandleDocWrite2D handle, ImgDoc2ErrorInformation* error_information)
{
    return IDocWriter2d_TransactionCommon(handle, error_information, &IDatabaseTransaction::CommitTransaction);
}

ImgDoc2ErrorCode IDocWrite2d_RollbackTransaction(HandleDocWrite2D handle, ImgDoc2ErrorInformation* error_information)
{
    return IDocWriter2d_TransactionCommon(handle, error_information, &IDatabaseTransaction::RollbackTransaction);
}

static ImgDoc2ErrorCode IDocWriter3d_TransactionCommon(HandleDocWrite3D handle, ImgDoc2ErrorInformation* error_information, void(IDatabaseTransaction::* mfp)())
{
    const auto writer3d_object = reinterpret_cast<SharedPtrWrapper<IDocWrite3d>*>(handle); // NOLINT(performance-no-int-to-ptr)
    if (!writer3d_object->IsValid())
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle("HandleDocRead3D", "The handle is invalid.", error_information);
        return ImgDoc2_ErrorCode_InvalidHandle;
    }

    const auto writer3d = writer3d_object->shared_ptr_;
    try
    {
        (writer3d.get()->*mfp)();
    }
    catch (exception& exception)
    {
        ImgDoc2ApiSupport::FillOutErrorInformation(exception, error_information);
        return ImgDoc2ApiSupport::MapExceptionToReturnValue(exception);
    }

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode IDocWrite3d_BeginTransaction(HandleDocWrite3D handle, ImgDoc2ErrorInformation* error_information)
{
    return IDocWriter3d_TransactionCommon(handle, error_information, &IDatabaseTransaction::BeginTransaction);
}

ImgDoc2ErrorCode IDocWrite3d_CommitTransaction(HandleDocWrite3D handle, ImgDoc2ErrorInformation* error_information)
{
    return IDocWriter3d_TransactionCommon(handle, error_information, &IDatabaseTransaction::CommitTransaction);
}

ImgDoc2ErrorCode IDocWrite3d_RollbackTransaction(HandleDocWrite3D handle, ImgDoc2ErrorInformation* error_information)
{
    return IDocWriter3d_TransactionCommon(handle, error_information, &IDatabaseTransaction::RollbackTransaction);
}
