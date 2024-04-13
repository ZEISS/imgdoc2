// SPDX-FileCopyrightText: 2024 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <exception>
#include "errorcodes.h"

class ImgDoc2ApiSupport
{
public:
    static void FillOutErrorInformation(const std::exception& exception, ImgDoc2ErrorInformation* error_information);
    static void FillOutErrorInformationForInvalidArgument(const char* argument_name, const char* text, ImgDoc2ErrorInformation* error_information);
    static void FillOutErrorInformationForInvalidHandle(const char* handle_name, const char* text, ImgDoc2ErrorInformation* error_information);
    static void FillOutErrorInformationForAllocationFailure(size_t required_size, ImgDoc2ErrorInformation* error_information);
    static ImgDoc2ErrorCode MapExceptionToReturnValue(const std::exception& exception);
};
