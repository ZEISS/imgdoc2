// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <exceptions.h>
#include <sstream>
#include <memory>
#include <vector>
#include <string>
#include <utility>
#include "sqlite_DbConnection.h"
#include "sqlite_DbStatement.h"
#include "custom_functions.h"
#include "loglevel.h"

using namespace std;
using namespace imgdoc2;

/*static*/std::shared_ptr<IDbConnection> SqliteDbConnection::SqliteCreateNewDatabase(const char* filename, std::shared_ptr<imgdoc2::IHostingEnvironment> environment)
{
    sqlite3* database = nullptr;
    const int return_value = sqlite3_open_v2(
        filename,
        &database,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI | SQLITE_OPEN_EXRESCODE,
        nullptr);

    if (return_value != SQLITE_OK)
    {
        // TODO(JBL): error handling might be more involved here, c.f. https://www.sqlite.org/c3ref/open.html
        if (database != nullptr)
        {
            // documentation states that even in case of an error, a database-connection object may be returned, which should be destroyed here
            sqlite3_close(database);
        }

        throw database_exception("Error from 'sqlite3_open_v2'", return_value);
    }

    return make_shared<SqliteDbConnection>(database, environment);
}

/*static*/std::shared_ptr<IDbConnection> SqliteDbConnection::SqliteOpenExistingDatabase(const char* filename, bool readonly, std::shared_ptr<imgdoc2::IHostingEnvironment> environment)
{
    sqlite3* database = nullptr;
    const int return_value = sqlite3_open_v2(
        filename,
        &database,
        (readonly ? SQLITE_OPEN_READONLY : SQLITE_OPEN_READWRITE) | SQLITE_OPEN_URI | SQLITE_OPEN_EXRESCODE,
        nullptr);

    if (return_value != SQLITE_OK)
    {
        // TODO(JBL): error handling might be more involved here, c.f. https://www.sqlite.org/c3ref/open.html
        if (database != nullptr)
        {
            // documentation states that even in case of an error, a database-connection object may be returned, which should be destroyed here
            sqlite3_close(database);
        }

        throw database_exception("Error from 'sqlite3_open_v2'", return_value);
    }

    return make_shared<SqliteDbConnection>(database, environment);
}

SqliteDbConnection::SqliteDbConnection(sqlite3* database, std::shared_ptr<imgdoc2::IHostingEnvironment> environment/*=nullptr*/)
    : environment_(std::move(environment)), database_(database), transaction_count_(0)
{
    SqliteCustomFunctions::SetupCustomQueries(database);
}

/*virtual*/SqliteDbConnection::~SqliteDbConnection()
{
    // Note: calling "sqlite3_close_v2" with nullptr is harmless
    sqlite3_close_v2(this->database_);
}

/*virtual*/void SqliteDbConnection::Execute(const char* sql_statement)
{
    // https://www.sqlite.org/c3ref/exec.html
    const int return_value = sqlite3_exec(this->database_, sql_statement, nullptr, nullptr, nullptr);
    this->LogSqlExecution("sqlite3_exec", sql_statement, return_value);
    if (return_value != SQLITE_OK)
    {
        throw database_exception("Error from 'sqlite3_exec'", return_value);
    }
}

/*virtual*/void SqliteDbConnection::Execute(IDbStatement* statement, std::int64_t* number_of_rows_modified/*=nullptr*/)
{
    if (statement == nullptr)
    {
        throw invalid_argument("The argument 'statement' must not be null.");
    }

    auto* sqlite_statement = dynamic_cast<ISqliteDbStatement*>(statement);
    if (sqlite_statement == nullptr)
    {
        throw imgdoc2_exception("Incorrect type encountered - object does not implement 'ISqliteDbStatement'-interface.");
    }

    const int return_value = sqlite3_step(sqlite_statement->GetSqliteSqlStatement());
    this->LogSqlExecution("sqlite3_step", sqlite_statement->GetSqliteSqlStatement(), return_value);

    // see https://www.sqlite.org/c3ref/step.html
    // Note that we intend that Execute-methods are used only for commands which do not return data,
    //  so this means that we do not expect 'SQLITE_ROW' here
    if (return_value != SQLITE_DONE)
    {
        throw database_exception("Error from 'sqlite3_step'", return_value);
    }

    if (number_of_rows_modified != nullptr)
    {
        *number_of_rows_modified = sqlite3_changes64(this->database_);
    }
}

/*virtual*/std::int64_t SqliteDbConnection::ExecuteAndGetLastRowId(IDbStatement* statement)
{
    this->Execute(statement);

    // https://www.sqlite.org/c3ref/last_insert_rowid.html
    const std::int64_t last_row_id = sqlite3_last_insert_rowid(this->database_);
    return last_row_id;
}

/*virtual*/std::shared_ptr<IDbStatement> SqliteDbConnection::PrepareStatement(const std::string& sql_statement)
{
    sqlite3_stmt* statement = nullptr;

    // https://www.sqlite.org/c3ref/prepare.html
    const int return_value = sqlite3_prepare_v2(
        this->database_,
        sql_statement.c_str(),
        -1,
        &statement,
        nullptr);
    if (return_value != SQLITE_OK || statement == nullptr)
    {
        throw database_exception("Error from 'sqlite3_prepare_v2'", return_value);
    }

    return make_shared<SqliteDbStatement>(statement);
}

/*virtual*/bool SqliteDbConnection::StepStatement(IDbStatement* statement)
{
    // try to cast "statement" to ISqliteStatement
    auto* sqlite_statement = dynamic_cast<ISqliteDbStatement*>(statement);
    if (sqlite_statement == nullptr)
    {
        throw runtime_error("incorrect type");
    }

    const int return_value = sqlite3_step(sqlite_statement->GetSqliteSqlStatement());
    this->LogSqlExecution("sqlite3_step", sqlite_statement->GetSqliteSqlStatement(), return_value);

    // https://www.sqlite.org/c3ref/step.html
    switch (return_value)
    {
        case SQLITE_ROW:
            return true;
        case SQLITE_DONE:
            return false;
        default:
            throw database_exception("Error from 'sqlite3_step'.", return_value);
    }
}

/*virtual*/void SqliteDbConnection::BeginTransaction()
{
    if (this->IsTransactionPending())
    {
        throw database_exception("Call to 'BeginTransaction' where there is already a pending transaction.");
    }

    this->Execute("BEGIN;");
    this->transaction_count_++;
}

/*virtual*/void SqliteDbConnection::EndTransaction(bool commit)
{
    if (!this->IsTransactionPending())
    {
        throw database_exception("Call to 'EndTransaction' where there is no pending transaction.");
    }

    const char* sql_command = (commit ? "COMMIT;" : "ROLLBACK;");

    this->Execute(sql_command);
    this->transaction_count_--;
}

/*virtual*/bool SqliteDbConnection::IsTransactionPending() const
{
    return this->transaction_count_ > 0;
}

/*virtual*/std::vector<IDbConnection::ColumnInfo> SqliteDbConnection::GetTableInfo(const char* table_name)
{
    ostringstream string_stream;
    string_stream << "SELECT name, type FROM pragma_table_info('" << table_name << "')";
    const auto statement = this->PrepareStatement(string_stream.str());

    vector<SqliteDbConnection::ColumnInfo> result;
    while (this->StepStatement(statement.get()))
    {
        ColumnInfo column_info;
        column_info.column_name = statement->GetResultString(0);
        column_info.column_type = statement->GetResultString(1);
        result.emplace_back(column_info);
    }

    return result;
}

/*virtual*/std::vector<IDbConnection::IndexInfo> SqliteDbConnection::GetIndicesOfTable(const char* table_name)
{
    ostringstream string_stream;
    string_stream << "SELECT name FROM pragma_index_list('" << table_name << "')";
    const auto statement = this->PrepareStatement(string_stream.str());
    vector<SqliteDbConnection::IndexInfo> result;
    while (this->StepStatement(statement.get()))
    {
        IndexInfo index_info;
        index_info.index_name = statement->GetResultString(0);
        result.emplace_back(index_info);
    }

    return result;
}

/*virtual*/const std::shared_ptr<imgdoc2::IHostingEnvironment>& SqliteDbConnection::GetHostingEnvironment() const
{
    return this->environment_;
}

void SqliteDbConnection::LogSqlExecution(const char* function_name, sqlite3_stmt* pStmt, int return_value) const
{
    if (this->GetHostingEnvironment()->IsLogLevelActive(LogLevel::Sql))
    {
        // https://www.sqlite.org/c3ref/expanded_sql.html -> may return NULL if the expansion fails (e.g. due to an OOM error)
        // (sqlite3_free is the correct function to free the memory allocated by sqlite3_expanded_sql, and it does nothing if called with NULL)
        const unique_ptr<char, decltype(sqlite3_free)*> up_expanded_sql_statement(sqlite3_expanded_sql(pStmt), sqlite3_free);
        if (up_expanded_sql_statement != nullptr)
        {
            this->LogSqlExecution(function_name, up_expanded_sql_statement.get(), return_value);
        }
        else
        {
            this->LogSqlExecution(function_name, "**expansion failed**", return_value);
        }
    }
}

void SqliteDbConnection::LogSqlExecution(const char* function_name, const char* sql_statement, int return_value) const
{
    if (this->GetHostingEnvironment()->IsLogLevelActive(LogLevel::Sql))
    {
        ostringstream string_stream;
        string_stream << "[" << function_name << "] -> (" << return_value << ", " << sqlite3_errstr(return_value) << "): " << sql_statement;
        this->GetHostingEnvironment()->Log(LogLevel::Sql, string_stream.str().c_str());
    }
}
