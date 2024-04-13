// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <IBlobOutput.h>

/// This interface is representing a "prepared/compiled" database-statement. The life-cycle of a database-statement is:
/// - an instance is created (-> IDbConnection::PrepareStatement)  
/// - if the statement is using literals which are to be replaced with parameters, then the appropriate Bind-methods are to be called  
/// - now the statement is sent to execution (-> IDbConnection::StepStatement)  
/// - and results may then be retrieved by calling into the appropriate "GetResult"-methods.  
/// This interface is intended to abstract different databases.
class IDbStatement
{
public:
    virtual void Reset() = 0;

    virtual void BindNull(int index) = 0;
    virtual void BindInt32(int index, std::int32_t value) = 0;
    virtual void BindInt64(int index, std::int64_t value) = 0;
    virtual void BindDouble(int index, double value) = 0;

    /// Bind a string (in UTF8). The string is copied (called "transient binding" in SQLite, c.f. https://www3.sqlite.org/c3ref/bind_blob.html).
    /// So, the string only must be valid for the duration of the execution of this method. However, there is of course a performance penalty
    /// here.
   ///
   /// \param  index   Index of the parameter to bind.
   /// \param  value   The null-terminated string to be bound (in UTF8 encoding).
    virtual void BindString(int index, const char* value) = 0;

    virtual void BindStringView(int index, const std::string_view& value) = 0;

    /// Bind a "static" BLOB. Static means that the pointer MUST remain valid until either the prepared statement is 
    /// finalized or the same SQL parameter is bound to something else. 
    ///
    /// \param  index   Index of the parameter to bind.
    /// \param  data    The pointer to the data.
    /// \param  size    The size of the data (in bytes).
    virtual void BindBlob_Static(int index, const void* data, size_t size) = 0;

    /// Gets the column of the result as an int32. This will coerce/convert that data into the desired type 'int32' if necessary.
    /// In particular, a DB-NULL is mapped to '0'.
    /// \param  column  The column.
    /// \returns    The value of the specified column.
    virtual std::int32_t GetResultInt32(int column) = 0;

    /// Gets the column of the result as an int32. This will convert that data into the desired type 'int32' if necessary.
    /// However, a DB-NULL is NOT mapped to '0', instead an empty result is returned.
    /// \param  column  The column.
    /// \returns    If it exists and is valid, the value of the specified column; otherwise an empty value.
    virtual std::optional<std::int32_t> GetResultInt32OrNull(int column) = 0;

    virtual std::uint32_t GetResultUInt32(int column) = 0;
    virtual std::uint8_t GetResultUInt8(int column) = 0;
    virtual std::int64_t GetResultInt64(int column) = 0;
    virtual double GetResultDouble(int column) = 0;

    virtual std::optional<double> GetResultDoubleOrNull(int column) = 0;
    virtual std::string GetResultString(int column) = 0;
    virtual void GetResultBlob(int column, imgdoc2::IBlobOutput* blobOutput) = 0;

    virtual ~IDbStatement() = default;

public:
    void BindString(int index, const std::string& value) { this->BindString(index, value.c_str()); }

public:
    // no copy and no move (-> https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all )
    IDbStatement() = default;
    IDbStatement(const IDbStatement&) = delete;             // copy constructor
    IDbStatement& operator=(const IDbStatement&) = delete;  // copy assignment
    IDbStatement(IDbStatement&&) = delete;                  // move constructor
    IDbStatement& operator=(IDbStatement&&) = delete;       // move assignment
};
