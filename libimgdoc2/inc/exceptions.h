// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdexcept>
#include <string>

#include "types.h"

namespace imgdoc2
{
    /// Base class for all imgdoc2-specific exceptions.
    class imgdoc2_exception : public std::runtime_error
    {
    public:
        /// Constructor.
        /// \param error_message Message describing the error.
        explicit imgdoc2_exception(const char* error_message)
            : std::runtime_error(error_message)
        {}
    };

    /// Exception for signalling database errors.
    class database_exception : public imgdoc2_exception
    {
    private:
        int sqlite_errorcode_;
        bool sqlite_errorcode_valid_;
    public:
        database_exception() = delete;

        /// Constructor.
        /// \param error_message Message describing the error.
        explicit database_exception(const char* error_message)
            : imgdoc2_exception(error_message), sqlite_errorcode_(-1), sqlite_errorcode_valid_(false)
        {}

        /// Constructor.
        /// \param  error_message     Message describing the error.
        /// \param  sqlite_error_code The SQLite error code.
        explicit database_exception(const char* error_message, int sqlite_error_code)
            : imgdoc2_exception(error_message), sqlite_errorcode_(sqlite_error_code), sqlite_errorcode_valid_(true)
        {}

        /// Gets a boolean indicating whether the SQLite error code is valid.
        /// \returns True if the SQLite error code is valid; false otherwise.
        bool GetIsSqliteErrorCodeValid() const { return this->sqlite_errorcode_valid_; }

        /// Gets the SQLite error code. Call database_exception::GetIsSqliteErrorCodeValid() in order to test whether the error code is valid.
        /// \returns The sqlite error code.
        int GetSqliteErrorCode() const { return this->sqlite_errorcode_; }

        /// Gets the SQLite provided error message for the error code. The result is unspecified if the error code is not valid.
        /// \returns The SQLite provided error message.
        std::string GetSqliteErrorMessage() const;
    };

    /// Exception for signalling that an operation is not allowed in the current state.
    class invalid_operation_exception : public imgdoc2_exception
    {
    public:
        /// Constructor.
        /// \param error_message Message describing the error.
        explicit invalid_operation_exception(const char* error_message)
            : imgdoc2_exception(error_message)
        {}
    };

    /// Exception for signalling logical errors during database-discovery.
    class discovery_exception : public imgdoc2_exception
    {
    public:
        /// Constructor.
        /// \param error_message Message describing the error.
        explicit discovery_exception(const char* error_message)
            : imgdoc2_exception(error_message)
        {}

        /// Constructor.
        /// \param error_message Message describing the error.
        explicit discovery_exception(const std::string& error_message)
            : discovery_exception(error_message.c_str())
        {}
    };

    /// Exception for signalling invalid arguments.
    class invalid_argument_exception : public imgdoc2_exception
    {
    public:
        /// Constructor.
        /// \param error_message Message describing the error.
        explicit invalid_argument_exception(const char* error_message)
            : imgdoc2_exception(error_message)
        {}
    };

    /// Exception for signalling that an attempt was made to access an non existing tile.
    class non_existing_tile_exception : public imgdoc2_exception
    {
    private:
        bool index_valid_{ false };
        imgdoc2::dbIndex index_{ 0 };
    public:
        non_existing_tile_exception() = delete;

        /// Constructor which specifies the primary key of the non existing tile.
        /// \param  error_message Message describing the error.
        /// \param  index         The primary key of the non existing tile.
        explicit non_existing_tile_exception(const std::string& error_message, imgdoc2::dbIndex index)
            : non_existing_tile_exception(error_message.c_str(), index)
        {}

        /// Constructor which specifies the primary key of the non existing tile.
        /// \param  error_message Message describing the error.
        /// \param  index         The primary key of the non existing tile.
        explicit non_existing_tile_exception(const char* error_message, imgdoc2::dbIndex index)
            : imgdoc2_exception(error_message),
            index_valid_(true),
            index_(index)
        {}

        /// Gets a boolean indicating whether the primary key of the faulty tile access is valid.
        /// \returns True if the primary key is valid; false otherwise.
        [[nodiscard]] bool IsIndexValid() const { return this->index_valid_; }

        /// Gets the primary key of the non existing tile which was attempted to be accessed. Check non_existing_tile_exception::IsIndexValid() whether
        /// this property is valid.
        /// \returns The index of the non existing tile which was attempted to be accessed.
        [[nodiscard]] imgdoc2::dbIndex GetIndex() const { return this->index_; }
    };

    /// Exception for signalling that an invalid path was specified.
    class invalid_path_exception : public imgdoc2_exception
    {
    public:
        invalid_path_exception() = delete;

        /// Constructor which specifies the primary key of the non existing tile.
        /// \param  error_message Message describing the error.
        explicit invalid_path_exception(const std::string& error_message)
            : imgdoc2_exception(error_message.c_str())
        {}

        /// Constructor which specifies the primary key of the non existing tile.
        /// \param  error_message Message describing the error.
        explicit invalid_path_exception(const char* error_message)
            : imgdoc2_exception(error_message)
        {}
    };

    /// Exception for signalling that an attempt was made to access an non existing metadata item.
    class non_existing_item_exception : public imgdoc2_exception
    {
    private:
        bool index_valid_{ false };
        imgdoc2::dbIndex index_{ 0 };
    public:
        non_existing_item_exception() = delete;

        /// Constructor which specifies the primary key of the non existing tile.
        /// \param  error_message Message describing the error.
        /// \param  index         The primary key of the non existing tile.
        explicit non_existing_item_exception(const std::string& error_message, imgdoc2::dbIndex index)
            : non_existing_item_exception(error_message.c_str(), index)
        {}

        /// Constructor which specifies the primary key of the non existing tile.
        /// \param  error_message Message describing the error.
        /// \param  index         The primary key of the non existing tile.
        explicit non_existing_item_exception(const char* error_message, imgdoc2::dbIndex index)
            : imgdoc2_exception(error_message),
            index_valid_(true),
            index_(index)
        {}

        /// Gets a boolean indicating whether the primary key of the faulty item access is valid.
        /// \returns True if the primary key is valid; false otherwise.
        [[nodiscard]] bool IsIndexValid() const { return this->index_valid_; }

        /// Gets the primary key of the non existing item which was attempted to be accessed. Check non_existing_item_exception::IsIndexValid() whether
        /// this property is valid.
        /// \returns The index of the non existing item which was attempted to be accessed.
        [[nodiscard]] imgdoc2::dbIndex GetIndex() const { return this->index_; }
    };

    /// Exception for signalling an unexpected internal error condition.
    class internal_error_exception : public imgdoc2_exception
    {
    public:
        internal_error_exception() = delete;

        /// Constructor.
        /// \param error_message Message describing the error.
        explicit internal_error_exception(const std::string& error_message)
            : imgdoc2_exception(error_message.c_str())
        {}
    };
}
