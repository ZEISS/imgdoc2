// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <sqlite3.h>

class ISqliteDbStatement
{
public:
    virtual sqlite3_stmt* GetSqliteSqlStatement() = 0;

    virtual ~ISqliteDbStatement() = default;
};
