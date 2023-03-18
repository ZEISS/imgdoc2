// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <memory>
#include <imgdoc.h>

/// This interface is representing a "database connection", it is a factory for creating objects
/// which allow to interact with the database. This "interaction" is strictly separated into
/// read-only- and modify-access.
/// TODO(JBL): * rules for concurrency are to be determined (and implemented)
class IDb
{
public:
    virtual std::shared_ptr<imgdoc::IDbWrite> GetWriter() = 0;
    virtual std::shared_ptr<imgdoc::IDbRead> GetReader() = 0;

};
