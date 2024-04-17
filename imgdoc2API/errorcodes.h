// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cstddef>

/// Defines an alias representing the error codes for the imgdoc2-module.
/// In general, values greater than zero indicate an error condition; and values less
/// or equal to zero indicate proper operation.
typedef int ImgDoc2ErrorCode;

/// The operation completed successfully.
static constexpr ImgDoc2ErrorCode ImgDoc2_ErrorCode_OK = 0;

/// An invalid argument was supplied to the function.
static constexpr ImgDoc2ErrorCode ImgDoc2_ErrorCode_InvalidArgument = 1;

/// An invalid handle was supplied to the function (i.e. a handle which is either a bogus value or a handle which has already been destroyed).
static constexpr ImgDoc2ErrorCode ImgDoc2_ErrorCode_InvalidHandle = 2;

/// A memory allocation error occurred (this could be in a caller-supplied callback function).
static constexpr ImgDoc2ErrorCode ImgDoc2_ErrorCode_AllocationError = 3;

/// An invalid id was specified when reading/accessing a tile.
static constexpr ImgDoc2ErrorCode ImgDoc2_Invalid_TileId = 5;   

/// An unspecified error occurred.
static constexpr ImgDoc2ErrorCode ImgDoc2_ErrorCode_UnspecifiedError = 50;

#pragma pack(push, 4)
struct ImgDoc2ErrorInformation
{
    static constexpr size_t kMaxMessageLength = 200;

    // TODO(JBL): idea is to return additional information about an error here
    char message[kMaxMessageLength];
};
#pragma pack(pop)
