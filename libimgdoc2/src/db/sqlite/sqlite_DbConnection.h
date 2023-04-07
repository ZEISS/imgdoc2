// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <memory>
#include <vector>
#include <string>
#include <sqlite3.h>
#include <IEnvironment.h>
#include "../IDbConnection.h"

/// Implementation of the IDbConnection-interface specific to SQLite.
class SqliteDbConnection : public IDbConnection
{
private:
    std::shared_ptr<imgdoc2::IHostingEnvironment> environment_;
    sqlite3* database_;
    int transaction_count_;
public:
    explicit SqliteDbConnection(sqlite3* database, std::shared_ptr<imgdoc2::IHostingEnvironment> environment = nullptr);
    SqliteDbConnection() = delete;

    static std::shared_ptr<IDbConnection> SqliteCreateNewDatabase(const char* filename, std::shared_ptr<imgdoc2::IHostingEnvironment> environment);
    static std::shared_ptr<IDbConnection> SqliteOpenExistingDatabase(const char* filename, bool readonly, std::shared_ptr<imgdoc2::IHostingEnvironment> environment);

    void Execute(const char* sql_statement) override;
    void Execute(IDbStatement* statement) override;
    std::int64_t ExecuteAndGetLastRowId(IDbStatement* statement) override;
    std::shared_ptr<IDbStatement> PrepareStatement(const std::string& sql_statement) override;

    /// Evaluate the statement and retrieve one row of results. This method can be called multiple times in order to return
    /// additional rows. The return value is true if a row was successfully retrieved and is available, and it is false
    /// if there is no more data available. All other error conditions result in an exception. 
    /// \param [in,out] statement The statement to evaluate and gather results.
    /// \returns True if a row of results was successfully retrieve; false if there is no more data available.
    bool StepStatement(IDbStatement* statement) override;

    void BeginTransaction() override;
    void EndTransaction(bool commit) override;
    bool IsTransactionPending() const override;

    std::vector<IDbConnection::ColumnInfo> GetTableInfo(const char* table_name) override;
    std::vector<IDbConnection::IndexInfo> GetIndicesOfTable(const char* table_name) override;

    [[nodiscard]] const std::shared_ptr<imgdoc2::IHostingEnvironment>& GetHostingEnvironment() const override;

    ~SqliteDbConnection() override;

private:
    void LogSqlExecution(const char* function_name, sqlite3_stmt* pStmt, int return_value) const;
    void LogSqlExecution(const char* function_name, const char* sql_statement, int return_value) const;
};
