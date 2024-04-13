// SPDX-FileCopyrightText: 2024 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include "codecsAPI.h"
#include <memory>
#include <libCZI.h>
#include "imgdoc2APIsupport.h"

using namespace libCZI;
using namespace std;

namespace
{
    libCZI::PixelType ConvertToLibCziPixelType(const std::uint8_t pixel_type)
    {
        switch (pixel_type)
        {
            case imgdoc2::PixelType::Gray8:
                return libCZI::PixelType::Gray8;
            case imgdoc2::PixelType::Gray16:
                return libCZI::PixelType::Gray16;
            case imgdoc2::PixelType::Bgr24:
                return libCZI::PixelType::Bgr24;
            case imgdoc2::PixelType::Bgr48:
                return libCZI::PixelType::Bgr48;
            case imgdoc2::PixelType::Gray32Float:
                return libCZI::PixelType::Gray32Float;
            default:
                return libCZI::PixelType::Invalid;
        }
    }

    void CopyWithStrideConversion(const void* source_data, std::uint32_t source_stride, libCZI::PixelType pixel_type, std::uint32_t width, std::uint32_t height, void* destination_data, std::uint32_t destination_stride)
    {
        const size_t line_length = width * static_cast<size_t>(libCZI::Utils::GetBytesPerPixel(pixel_type));

        // Iterate over each row of the source image
        for (std::uint32_t row = 0; row < height; row++)
        {
            // Calculate the memory address of the current row in the source and destination images
            const std::uint8_t* source_row = static_cast<const std::uint8_t*>(source_data) + static_cast<size_t>(row) * source_stride;
            std::uint8_t* destination_row = static_cast<std::uint8_t*>(destination_data) + static_cast<size_t>(row) * destination_stride;

            // Copy the row from the source image to the destination image
            std::memcpy(destination_row, source_row, line_length);
        }
    }
}

ImgDoc2ErrorCode DecodeImage(
                const BitmapInfoInterop* bitmap_info,
                std::uint8_t data_type,
                const void* compressed_data,
                std::uint64_t compressed_data_size,
                std::uint32_t destination_stride,
                AllocMemoryFunctionPointer allocate_memory_function,
                DecodedImageResultInterop* result,
                ImgDoc2ErrorInformation* error_information)
{
    if (bitmap_info == nullptr)
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument("bitmap_info", "must not be null", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    if (bitmap_info->pixelWidth == 0 || bitmap_info->pixelHeight == 0)
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument("bitmap_info", "pixelWidth and pixelHeight must be greater than 0", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    if (compressed_data_size == 0)
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument("compressed_data_size", "must be greater than 0", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    if (compressed_data == nullptr)
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument("compressed_data", "must not be null", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    if (allocate_memory_function == nullptr)
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument("allocate_memory_function", "must not be null", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    if (result == nullptr)
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument("result", "must not be null", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    if (destination_stride > 0)
    {
        if (destination_stride < bitmap_info->pixelWidth * libCZI::Utils::GetBytesPerPixel(ConvertToLibCziPixelType(bitmap_info->pixelType)))
        {
            ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument("destination_stride", "must be either be zero (which means that the stride is chosen by this function) or greater than or equal to pixelWidth * bytes per pixel", error_information);
            return ImgDoc2_ErrorCode_InvalidArgument;
        }
    }

    const libCZI::PixelType libczi_pixel_type = ConvertToLibCziPixelType(bitmap_info->pixelType);
    if (libczi_pixel_type == libCZI::PixelType::Invalid)
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument("bitmap_info", "pixelType is not supported", error_information);
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    std::shared_ptr<libCZI::IBitmapData> decoded_bitmap;

    switch (data_type)
    {
        case static_cast<std::underlying_type_t<imgdoc2::DataTypes>>(imgdoc2::DataTypes::JPGXRCOMPRESSED_BITMAP):
            try
            {
                const auto decoder = libCZI::GetDefaultSiteObject(libCZI::SiteObjectType::Default)->GetDecoder(ImageDecoderType::JPXR_JxrLib, nullptr);
                decoded_bitmap = decoder->Decode(compressed_data, compressed_data_size, libczi_pixel_type, bitmap_info->pixelWidth, bitmap_info->pixelHeight);
            }
            catch (const std::exception& e)
            {
                ImgDoc2ApiSupport::FillOutErrorInformation(e, error_information);
                return ImgDoc2_ErrorCode_UnspecifiedError;
            }

            break;
        case static_cast<std::underlying_type_t<imgdoc2::DataTypes>>(imgdoc2::DataTypes::ZSTD0COMPRESSED_BITMAP):
            try
            {
                const auto decoder = libCZI::GetDefaultSiteObject(libCZI::SiteObjectType::Default)->GetDecoder(ImageDecoderType::ZStd0, nullptr);
                decoded_bitmap = decoder->Decode(compressed_data, compressed_data_size, libczi_pixel_type, bitmap_info->pixelWidth, bitmap_info->pixelHeight);
            }
            catch (const std::exception& e)
            {
                ImgDoc2ApiSupport::FillOutErrorInformation(e, error_information);
                return ImgDoc2_ErrorCode_UnspecifiedError;
            }

            break;
        case static_cast<std::underlying_type_t<imgdoc2::DataTypes>>(imgdoc2::DataTypes::ZSTD1COMPRESSED_BITMAP):
            try
            {
                const auto decoder = libCZI::GetDefaultSiteObject(libCZI::SiteObjectType::Default)->GetDecoder(ImageDecoderType::ZStd1, nullptr);
                decoded_bitmap = decoder->Decode(compressed_data, compressed_data_size, libczi_pixel_type, bitmap_info->pixelWidth, bitmap_info->pixelHeight);
            }
            catch (const std::exception& e)
            {
                ImgDoc2ApiSupport::FillOutErrorInformation(e, error_information);
                return ImgDoc2_ErrorCode_UnspecifiedError;
            }

            break;
        default:
            ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument("data_type", "is not supported", error_information);
            return ImgDoc2_ErrorCode_InvalidArgument;
    }

    const ScopedBitmapLockerSP decoder_bitmap_locker(decoded_bitmap);

    // the stride for the decoded image is either the one we happen to get from the decoder (in the case that no stride was passed in) or the one that was passed in
    result->stride = destination_stride == 0 ? decoder_bitmap_locker.stride : destination_stride;

    const uint64_t required_size = static_cast<uint64_t>(result->stride) * bitmap_info->pixelHeight;

    const bool success = allocate_memory_function(required_size, &result->bitmap);
    if (!success)
    {
        ImgDoc2ApiSupport::FillOutErrorInformationForAllocationFailure(required_size, error_information);
        return ImgDoc2_ErrorCode_AllocationError;
    }

    CopyWithStrideConversion(
        decoder_bitmap_locker.ptrDataRoi,
        decoder_bitmap_locker.stride,
        decoded_bitmap->GetPixelType(),
        bitmap_info->pixelWidth,
        bitmap_info->pixelHeight,
        result->bitmap.pointer_to_memory,
        result->stride);

    return ImgDoc2_ErrorCode_OK;
}
