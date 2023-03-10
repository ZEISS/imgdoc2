// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <exceptions.h>
#include <sqlite3.h>

using namespace std;
using namespace imgdoc2;

std::string imgdoc2::database_exception::GetSqliteErrorMessage() const
{
    if (this->GetIsSqliteErrorCodeValid())
    {
        return { sqlite3_errstr(this->GetSqliteErrorCode()) };
    }

    return { "No Sqlite-Errorcode available." };
}
