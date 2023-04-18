// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <string>

namespace imgdoc2
{
    /// This interface gathers all parameters for the operation of "opening an existing file".
    class IOpenExistingOptions
    {
    public:
        /// Sets the filename of the file to be opened.
        /// \param  filename The null-terminated string specifying the file to be opened. The string
        ///                   is expected to be in UTF8-encoding.
        virtual void SetFilename(const char* filename) = 0;

        /// Sets whether the file is to opened as "readonly".
        /// \param  read_only True file is to opened as "readonly".
        virtual void SetOpenReadonly(bool read_only) = 0;

        /// Gets a boolean indicating whether the file is to be opened as "readonly".
        /// \returns True if the file is to be opened as "readonly"; false otherwise.
        [[nodiscard]] virtual bool GetOpenReadonly() const = 0;

        /// Gets the filename. The string is given in UTF8-encoding.
        /// \returns The filename (in UTF8-encoding).
        [[nodiscard]] virtual const std::string& GetFilename() const = 0;

        virtual ~IOpenExistingOptions() = default;

        /// Sets the filename of the file to be opened.
        /// \param  filename The null-terminated string specifying the file to be opened. The string
        ///                   is expected to be in UTF8-encoding.
        void SetFilename(const std::string& filename)
        {
            this->SetFilename(filename.c_str());
        }

        // no copy and no move (-> https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all )
        IOpenExistingOptions() = default;
        IOpenExistingOptions(const IOpenExistingOptions&) = delete;             // copy constructor
        IOpenExistingOptions& operator=(const IOpenExistingOptions&) = delete;  // copy assignment
        IOpenExistingOptions(IOpenExistingOptions&&) = delete;                  // move constructor
        IOpenExistingOptions& operator=(IOpenExistingOptions&&) = delete;       // move assignment
    };
}
