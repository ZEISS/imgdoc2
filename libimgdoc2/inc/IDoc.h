// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <memory>

namespace imgdoc2
{
    class IDocWrite2d;
    class IDocRead2d;

    /// Values that represent different document types.
    enum class DocumentType
    {
        kInvalid = 0,    ///< An enum constant representing the invalid option.
        kImage2d,        ///< An enum constant representing the "image 2D" option.
        kImage3d         ///< An enum constant representing the "image 3D" option. NOT YET IMPLEMENTED.
    };

    /// This interface is representing a 'document'. The discovery phase of the document has been completed successfully.
    /// Depending on the type of the document, objects for interacting with it can be created.
    class IDoc
    {
    public:
        /// Try to get a "write object" for a 2D-document. This method may return an empty shared_ptr
        /// if such an object cannot be constructed.
        /// \returns The writer-object (for 2D-document).
        virtual std::shared_ptr<imgdoc2::IDocWrite2d> GetWriter2d() = 0;

        /// Try to get a "read object" for a 2D-document. This method may return an empty shared_ptr
        /// if such an object cannot be constructed.
        /// \returns The read-object (for 2D-document).
        virtual std::shared_ptr<imgdoc2::IDocRead2d> GetReader2d() = 0;

        virtual ~IDoc() = default;

    public:
        // no copy and no move (-> https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all )
        IDoc() = default;
        IDoc(const IDoc&) = delete;             // copy constructor
        IDoc& operator=(const IDoc&) = delete;  // copy assignment
        IDoc(IDoc&&) = delete;                  // move constructor
        IDoc& operator=(IDoc&&) = delete;       // move assignment
    };
}
