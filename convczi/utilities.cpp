// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include "utilities.h"
#include <memory>
#include <cstdlib>

#if CONVCZI_WIN32_ENVIRONMENT
#include <Windows.h>
#endif

using namespace std;

#if CONVCZI_WIN32_ENVIRONMENT
CommandlineArgsWindowsHelper::CommandlineArgsWindowsHelper()
{
    int number_arguments;
    const unique_ptr<LPWSTR, decltype(LocalFree)*> wide_argv
    {
        CommandLineToArgvW(GetCommandLineW(), &number_arguments),
        &LocalFree
    };

    this->pointers_to_arguments_.reserve(number_arguments);
    this->arguments_.reserve(number_arguments);

    for (int i = 0; i < number_arguments; ++i)
    {
        this->arguments_.emplace_back(convertToUtf8(wide_argv.get()[i]));
    }

    for (int i = 0; i < number_arguments; ++i)
    {
        this->pointers_to_arguments_.emplace_back(
            this->arguments_[i].data());
    }
}

char** CommandlineArgsWindowsHelper::GetArgv()
{
    return this->pointers_to_arguments_.data();
}

int CommandlineArgsWindowsHelper::GetArgc()
{
    return static_cast<int>(this->pointers_to_arguments_.size());
}
#endif

std::string convertToUtf8(const std::wstring& str)
{
    // see here for the state of this business... https://stackoverflow.com/questions/402283/stdwstring-vs-stdstring
#if CONVCZI_WIN32_ENVIRONMENT
    const int count = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
    string converted(count, 0);
    WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, converted.data(), count, NULL, NULL);
    return converted;
#endif
#if CONVCZI_UNIX_ENVIRONMENT
    size_t requiredSize = std::wcstombs(nullptr, str.c_str(), 0);
    std::string converted(requiredSize, 0);
    converted.resize(std::wcstombs(converted.data(), str.c_str(), requiredSize));
    return converted;
#endif
}

std::wstring convertToWide(const std::string& str)
{
#if CONVCZI_WIN32_ENVIRONMENT
    const int count = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    std::wstring converted(count, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, converted.data(), count);
    return converted;
#endif
#if CONVCZI_UNIX_ENVIRONMENT
    std::wstring converted(str.size(), 0);  // here we assume that the converted wstring can have at most as many characters as the UTF8-source string has
    size_t size = std::mbstowcs(converted.data(), str.c_str(), str.size());
    converted.resize(size);
    return converted;
#endif
}
