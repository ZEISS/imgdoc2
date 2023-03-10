// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include "ConvCZI_Config.h"
#include <vector>
#include <string>

#if CONVCZI_WIN32_ENVIRONMENT
/// A utility which is providing the command-line arguments (on Windows) as UTF8-encoded strings.
class CommandlineArgsWindowsHelper
{
private:
    std::vector<std::string> arguments_;
    std::vector<char*> pointers_to_arguments_;
public:
    /// Constructor.
    CommandlineArgsWindowsHelper();

    /// Gets an array of pointers to null-terminated, UTF8-encoded arguments. This size of this array is given
    /// by the "GetArgc"-method.
    /// Note that this pointer is only valid for the lifetime of this instance of the CommandlineArgsWindowsHelper-class.
    ///
    /// \returns    Pointer to an array of pointers to null-terminated, UTF8-encoded arguments.
    char** GetArgv();

    /// Gets the number of arguments.
    ///
    /// \returns    The number of arguments.
    int GetArgc();
};
#endif

/// Converts the specified wide string to its UTF8-representation.
/// \param str  The (wide) source string.
/// \returns The given string converted to an UTF8-representation.
std::string convertToUtf8(const std::wstring& str);

/// Converts the UTF8-encoded string 'str' to a wide-string representation.
///
/// \param  str The UTF8-encode string to convert.
///
/// \returns    The given data converted to a wide string.
std::wstring convertToWide(const std::string& str);
