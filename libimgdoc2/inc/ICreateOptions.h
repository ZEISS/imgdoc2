// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <unordered_set>
#include "types.h"

namespace imgdoc2
{
    /// Options for creating an imgdoc2-document.
    class ICreateOptions
    {
    public:
        /// Sets the filename. For a Sqlite-based database, this string allows for additional functionality
        /// (like an in-memory database) - cf. https://sqlite.org/inmemorydb.html, https://sqlite.org/uri.html.
        /// The string must be given in UTF-8 encoding.
        ///
        /// \param  filename  The filename (in UTF8-encoding).
        virtual void SetFilename(const char* filename) = 0;

        /// Adds a dimension.
        /// Adding the same dimension multiple times is valid, no error is reported in this case.
        /// If the argument 'dim' is not a valid dimension identifier, an "invalid_argument" exception
        /// will be thrown.
        /// \param  dim The dimension to add.
        virtual void AddDimension(imgdoc2::Dimension dim) = 0;

        /// Sets a flag indicating whether the database should be created containing a spatial index.
        /// \param  use_spatial_index True if to construct with a spatial index.
        virtual void SetUseSpatialIndex(bool use_spatial_index) = 0;

        /// Adds a dimension for which an index is to be created.
        /// Adding the same dimension multiple times is valid, no error is reported in this case.
        /// If the argument 'dim' is not a valid dimension identifier, an "invalid_argument" exception
        /// will be thrown.
        /// \param  dim The dimension for which to create an index.
        virtual void AddIndexForDimension(imgdoc2::Dimension dim) = 0;

        /// Sets a flag indicating whehter a BLOB table is to be constructed. Only if a BLOB table is present, the storage-type "BlobInDatabase" can be used.
        /// \param  create_blob_table True to create BLOB table.
        virtual void SetCreateBlobTable(bool create_blob_table) = 0;

        /// Whether the document should be created with a spatial index.
        /// \returns True if a spatial index is requested; false otherwise.
        [[nodiscard]] virtual bool GetUseSpatialIndex() const = 0;

        /// Gets the filename. The returned string is given in UTF8-encoding.
        /// \returns The filename.
        [[nodiscard]] virtual const std::string& GetFilename() const = 0;

        /// Gets the dimensions.
        /// \returns The dimensions.
        [[nodiscard]] virtual const std::unordered_set<imgdoc2::Dimension>& GetDimensions() const = 0;

        /// Gets dimensions for which an index is to be created.
        /// \returns The indexed dimensions.
        [[nodiscard]] virtual const std::unordered_set<imgdoc2::Dimension>& GetIndexedDimensions() const = 0;

        /// Gets a boolean indicating whether a blob table is to be created.
        /// \returns True if a blob table is to be created; false otherwise.
        [[nodiscard]] virtual bool GetCreateBlobTable() const = 0;

        virtual ~ICreateOptions() = default;
    public:
        /// Adds the dimensions from the specified iterator.
        /// \tparam ForwardIterator Type of the forward iterator.
        /// \param  start The start.
        /// \param  end   The end.
        template<class ForwardIterator>
        void AddDimensions(ForwardIterator start, ForwardIterator end)
        {
            while (start != end)
            {
                this->AddDimension(*start++);
            }
        }
    };
}
