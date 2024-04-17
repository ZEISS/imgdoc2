// SPDX-FileCopyrightText: 2024 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include "imgdoc2APIsupport.h"
#include "utilities.h"

/*static*/void ImgDoc2ApiSupport::FillOutErrorInformation(const std::exception& exception, ImgDoc2ErrorInformation* error_information)
{
    if (error_information != nullptr)
    {
        const auto error_message = exception.what();

        // ensure that the string is always null-terminated, even in the case of truncation
        Utilities::copy_string_to_fixed_size(error_message, error_information->message, ImgDoc2ErrorInformation::kMaxMessageLength);
    }
}

/*static*/void ImgDoc2ApiSupport::FillOutErrorInformationForInvalidArgument(const char* argument_name, const char* text, ImgDoc2ErrorInformation* error_information)
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

/*static*/void ImgDoc2ApiSupport::FillOutErrorInformationForInvalidHandle(const char* handle_name, const char* text, ImgDoc2ErrorInformation* error_information)
{
    if (error_information != nullptr)
    {
        stringstream ss;
        ss << "Handle '" << handle_name << "'";
        if (text != nullptr)
        {
            ss << ": " << text;
        }

        Utilities::copy_string_to_fixed_size(ss.str().c_str(), error_information->message, ImgDoc2ErrorInformation::kMaxMessageLength);
    }
}

/*static*/void ImgDoc2ApiSupport::FillOutErrorInformationForAllocationFailure(size_t required_size, ImgDoc2ErrorInformation* error_information)
{
    if (error_information != nullptr)
    {
        stringstream ss;
        ss << "Allocation failure: " << required_size << " bytes required";

        Utilities::copy_string_to_fixed_size(ss.str().c_str(), error_information->message, ImgDoc2ErrorInformation::kMaxMessageLength);
    }
}

/*static*/ImgDoc2ErrorCode ImgDoc2ApiSupport::MapExceptionToReturnValue(const std::exception& exception)
{
    if (typeid(exception) == typeid(invalid_argument))
    {
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    if (typeid(exception) == typeid(imgdoc2::non_existing_tile_exception))
    {
        return ImgDoc2_Invalid_TileId;
    }

    // TODO(Jbl) - add additional exception types

    return ImgDoc2_ErrorCode_UnspecifiedError;
}
