// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <memory>
#include <IEnvironment.h>
#include "IDbConnection.h"

class DbFactory
{
public:
    static std::shared_ptr<IDbConnection> SqliteCreateNewDatabase(const char* filename, std::shared_ptr<imgdoc2::IHostingEnvironment> environment = nullptr);
    static std::shared_ptr<IDbConnection> SqliteOpenExistingDatabase(const char* filename, bool readonly, std::shared_ptr<imgdoc2::IHostingEnvironment> environment = nullptr);
};
