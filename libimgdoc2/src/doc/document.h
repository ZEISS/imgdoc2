// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <utility>
#include <memory>
#include <imgdoc2.h>
#include "../db/IDbConnection.h"
#include "../db/database_configuration.h"

class Document : public imgdoc2::IDoc, public std::enable_shared_from_this<Document>
{
private:
    std::shared_ptr<IDbConnection> database_connection_;
    std::shared_ptr<DatabaseConfiguration2D> database_configuration_2d_;
public:
    Document(std::shared_ptr<IDbConnection> database_connection, std::shared_ptr<DatabaseConfiguration2D> database_configuration) :
        database_connection_(std::move(database_connection)),
        database_configuration_2d_(std::move(database_configuration))
    {}

    /// Try to get a "write object" for a 2D-document. This method may return an empty shared_ptr
    /// if such an object cannot be constructed.
    /// \returns The writer-object (for 2D-document).
    std::shared_ptr<imgdoc2::IDocWrite2d> GetWriter2d() override;

    /// Try to get a "read object" for a 2D-document. This method may return an empty shared_ptr
    /// if such an object cannot be constructed.
    /// \returns The read-object (for 2D-document).
    std::shared_ptr<imgdoc2::IDocRead2d> GetReader2d() override;

    /// Try to get a "write object" for a 3D-document. This method may return an empty shared_ptr
    /// if such an object cannot be constructed.
    /// \returns The writer-object (for 3D-document).
    std::shared_ptr<imgdoc2::IDocWrite3d> GetWriter3d() override;

    /// Try to get a "read object" for a 3D-document. This method may return an empty shared_ptr
    /// if such an object cannot be constructed.
    /// \returns The read-object (for 3D-document).
    std::shared_ptr<imgdoc2::IDocRead3d> GetReader3d() override;

    ~Document() override = default;
public:
    [[nodiscard]] const std::shared_ptr<IDbConnection>& GetDatabase_connection() const { return this->database_connection_; }
    [[nodiscard]] const std::shared_ptr<DatabaseConfiguration2D>& GetDataBaseConfiguration2d() const { return this->database_configuration_2d_; }

    [[nodiscard]] const std::shared_ptr<imgdoc2::IHostingEnvironment>& GetHostingEnvironment() const { return this->database_connection_->GetHostingEnvironment(); }
};
