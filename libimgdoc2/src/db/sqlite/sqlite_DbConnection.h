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
    bool StepStatement(IDbStatement* statement) override;

    void BeginTransaction() override;
    void EndTransaction(bool commit) override;
    bool IsTransactionPending() const override;

    std::vector<IDbConnection::ColumnInfo> GetTableInfo(const char* table_name) override;
    std::vector<IDbConnection::IndexInfo> GetIndicesOfTable(const char* table_name) override;

    [[nodiscard]] const std::shared_ptr<imgdoc2::IHostingEnvironment>& GetHostingEnvironment() const override;

    ~SqliteDbConnection() override;
};
