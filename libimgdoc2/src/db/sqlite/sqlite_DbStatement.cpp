// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include "sqlite_DbStatement.h"
#include <exceptions.h>
#include <sstream>

using namespace std;
using namespace imgdoc2;

SqliteDbStatement::SqliteDbStatement(sqlite3_stmt* sql_statement)  :
    sql_statement_(sql_statement)
{
}

/*virtual*/SqliteDbStatement::~SqliteDbStatement()
{
    // TODO(JBL): check error (-> https://www.sqlite.org/c3ref/finalize.html)
    sqlite3_finalize(this->sql_statement_);
}

/*virtual*/void SqliteDbStatement::Reset()
{
    throw std::logic_error("not implemented");
}

/*virtual*/void SqliteDbStatement::BindNull(int index)
{
    const int return_code = sqlite3_bind_null(this->sql_statement_, index);
    this->ThrowIfBindError(return_code, "sqlite3_bind_null");
}

/*virtual*/void SqliteDbStatement::BindInt32(int index, std::int32_t value)
{
    const int return_code = sqlite3_bind_int(this->sql_statement_, index, value);
    this->ThrowIfBindError(return_code, "sqlite3_bind_int");
}

/*virtual*/void SqliteDbStatement::BindInt64(int index, std::int64_t value)
{
    const int return_code = sqlite3_bind_int64(this->sql_statement_, index, value);
    this->ThrowIfBindError(return_code, "sqlite3_bind_int64");
}

/*virtual*/void SqliteDbStatement::BindDouble(int index, double value)
{
    const int return_code = sqlite3_bind_double(this->sql_statement_, index, value);
    this->ThrowIfBindError(return_code, "sqlite3_bind_double");
}

/*virtual*/void SqliteDbStatement::BindString(int index, const char* value)
{
    // -1 means: determine length (assuming it is a null-terminated string)
    // SQLITE_TRANSIENT means: make a copy of the string
    const int return_code = sqlite3_bind_text(this->sql_statement_, index, value, -1, SQLITE_TRANSIENT);
    this->ThrowIfBindError(return_code, "sqlite3_bind_text");
}

/*virtual*/void SqliteDbStatement::BingStringView(int index, const std::string_view& value)
{
    const int return_code = sqlite3_bind_text(this->sql_statement_, index, value.data(), value.size(), SQLITE_TRANSIENT);
    this->ThrowIfBindError(return_code, "sqlite3_bind_text");
}

/*virtual*/void SqliteDbStatement::BindBlob_Static(int index, const void* data, size_t size)
{
    const int return_code = sqlite3_bind_blob64(this->sql_statement_, index, data, size, SQLITE_STATIC);
    this->ThrowIfBindError(return_code, "sqlite3_bind_blob64");
}

/*virtual*/sqlite3_stmt* SqliteDbStatement::GetSqliteSqlStatement() 
{
    return this->sql_statement_;
}

/*virtual*/std::int32_t SqliteDbStatement::GetResultInt32(int column)
{
    const std::int32_t value = sqlite3_column_int(this->sql_statement_, column);
    return value;
}

/*virtual*/std::optional<std::int32_t> SqliteDbStatement::GetResultInt32OrNull(int column)
{
    int32_t result = this->GetResultInt32(column);
    if (result == 0)
    {
        // a value of 0 **could** mean that we actually read a NULL (and it got coalesced into a 0) -> https://www.sqlite.org/c3ref/column_blob.html
        if (sqlite3_column_type(this->sql_statement_, column) == SQLITE_NULL)
        {
            return {};
        }
    }

    return result;
}

/*virtual*/std::int64_t SqliteDbStatement::GetResultInt64(int column)
{
    const int64_t value = sqlite3_column_int64(this->sql_statement_, column);
    return value;
}

/*virtual*/double SqliteDbStatement::GetResultDouble(int column)
{
    const double value = sqlite3_column_double(this->sql_statement_, column);
    return value;
}

/*virtual*/std::optional<double> SqliteDbStatement::GetResultDoubleOrNull(int column)
{
    const auto result = this->GetResultDouble(column);
    if (result == 0.0)
    {
        // a value of 0 **could** mean that we actually read a NULL (and it got coalesced into a 0) -> https://www.sqlite.org/c3ref/column_blob.html
        if (sqlite3_column_type(this->sql_statement_, column) == SQLITE_NULL)
        {
            return {};
        }
    }

    return result;
}

/*virtual*/std::uint32_t SqliteDbStatement::GetResultUInt32(int column)
{
    const uint32_t value = static_cast<uint32_t>(sqlite3_column_int(this->sql_statement_, column));
    return value;
}

/*virtual*/std::uint8_t SqliteDbStatement::GetResultUInt8(int column)
{
    const uint8_t value = static_cast<uint8_t>(sqlite3_column_int(this->sql_statement_, column));
    return value;
}

/*virtual*/void SqliteDbStatement::GetResultBlob(int column, imgdoc2::IBlobOutput* blobOutput)
{
    int size_of_blob = sqlite3_column_bytes(this->sql_statement_, column);
    if (blobOutput->Reserve(size_of_blob))
    {
        const void* ptr_data = sqlite3_column_blob(this->sql_statement_, column);
        blobOutput->SetData(0, size_of_blob, ptr_data);
    }
}

/*virtual*/std::string SqliteDbStatement::GetResultString(int column)
{
    // TODO(JBL): this method is creating a copy of the string, it might be a good idea to have a method
    //        which returns a "dangereous pointer"
    const unsigned char* str = sqlite3_column_text(this->sql_statement_, column);

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast) no way (I can see) to prevent the reinterpret_cast here
    return string{ reinterpret_cast<const char*>(str) };
}

void SqliteDbStatement::ThrowIfBindError(int error_code, const char* function_name)
{
    if (error_code != SQLITE_OK)
    {
        // TODO(JBL) : not exactly sure which error to throw
        // https://www.sqlite.org/c3ref/bind_blob.html
        ostringstream string_stream;
        string_stream << "Error binding a value (with function \"" << function_name << "\").";
        throw database_exception(string_stream.str().c_str(), error_code);
    }
}
