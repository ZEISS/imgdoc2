// SPDX-FileCopyrightText: 2024 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include "imgdoc2API.h"
#include "importexport.h"
#include "errorcodes.h"
#include "bitmapinfointerop.h"
#include "decodedimageresultinterop.h"

/// Decodes the specific compressed data into an uncompressed bitmap. This destination bitmap is allocated
/// by a user provided function. The caller may either provide a stride it expects the destination bitmap to be
/// given, or 0 to let the function determine and choose the stride itself. The function will return the stride it used in
/// the result structure. The caller is responsible for freeing the memory allocated by the allocate_memory_function.
///
/// \param          bitmap_info                 Information describing the (compressed) bitmap. If this information turns out to be not
///                                             corresponding to the actual data, the function will return an error.
/// \param          data_type                   The type of the compression (corresponding to imgdoc2::DataTypes).
/// \param          compressed_data             Pointer to the compressed data.
/// \param          compressed_data_size        Size of the compressed data in bytes.
/// \param          destination_stride          The destination stride or 0 to let the function determine the stride itself.
/// \param          allocate_memory_function    A function pointer which will be called to allocate the destination memory.
/// \param [out]    result                      If successful, the result is put here.
/// \param [in,out] error_information           If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns    An error-code indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) DecodeImage(
                const BitmapInfoInterop* bitmap_info,
                std::uint8_t data_type,
                const void* compressed_data,
                std::uint64_t compressed_data_size,
                std::uint32_t destination_stride,
                AllocMemoryFunctionPointer allocate_memory_function,
                DecodedImageResultInterop* result,
                ImgDoc2ErrorInformation* error_information);

