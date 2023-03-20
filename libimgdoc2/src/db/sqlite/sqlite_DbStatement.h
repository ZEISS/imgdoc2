// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <optional>
#include <sqlite3.h>
#include "../IDbConnection.h"
#include "ISqlite_DbStatement.h"

/// Implementation of the IDbStatement-interface specific for SQLite. Note that 
/// the DbStatement-implementation for SQLite is supporting an additional interface
/// ISqliteDbStatement.
class SqliteDbStatement : public IDbStatement, public ISqliteDbStatement
{
private:
    sqlite3_stmt* sql_statement_;
public:
    /// Constructor - this class takes ownership of the specified sql_statement.
    ///
    /// \param [in] sql_statement   The SQL statement, this object takes ownership of it.
    explicit SqliteDbStatement(sqlite3_stmt* sql_statement);

    ~SqliteDbStatement() override;

    void Reset() override;
    void BindNull(int index) override;
    void BindInt32(int index, std::int32_t value) override;
    void BindInt64(int index, std::int64_t value) override;
    void BindDouble(int index, double value) override;
    void BindString(int index, const char* value) override;

    void BindBlob_Static(int index, const void* data, size_t size) override;

    sqlite3_stmt* GetSqliteSqlStatement() override;

    std::int32_t GetResultInt32(int column) override;
    std::optional<std::int32_t> GetResultInt32OrNull(int column) override;
    std::int64_t GetResultInt64(int column) override;
    std::uint32_t GetResultUInt32(int column) override;
    std::uint8_t GetResultUInt8(int column) override;
    double GetResultDouble(int column) override;
    std::string GetResultString(int column) override;
    void GetResultBlob(int column, imgdoc2::IBlobOutput* blobOutput) override;

private:
    void ThrowIfBindError(int error_code, const char* function_name);

public:
    SqliteDbStatement() = delete;
};
