// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <utility>
#include <memory>
#include <imgdoc2.h>
#include "document.h"
#include "ITileCoordinate.h"

class DocumentWrite3d : public imgdoc2::IDocWrite3d
{
private:
    std::shared_ptr < Document> document_;
public:
    explicit DocumentWrite3d(std::shared_ptr<Document> document) : document_(std::move(document))
    {}

    imgdoc2::dbIndex AddBrick(
        const imgdoc2::ITileCoordinate* coord,
        const imgdoc2::LogicalPositionInfo3D* info,
        const imgdoc2::BrickBaseInfo* tileInfo,
        imgdoc2::DataTypes datatype,
        imgdoc2::TileDataStorageType storage_type,
        const imgdoc2::IDataObjBase* data) override;

    void BeginTransaction() override;
    void CommitTransaction() override;
    void RollbackTransaction() override;

    ~DocumentWrite3d() override = default;

private:
    imgdoc2::dbIndex AddBrickInternal(
        const imgdoc2::ITileCoordinate* coord,
        const imgdoc2::LogicalPositionInfo3D* info,
        const imgdoc2::BrickBaseInfo* tileInfo,
        imgdoc2::DataTypes datatype,
        imgdoc2::TileDataStorageType storage_type,
        const imgdoc2::IDataObjBase* data);

    //void AddToSpatialIndex(imgdoc2::dbIndex index, const imgdoc2::LogicalPositionInfo& logical_position_info);

    //imgdoc2::dbIndex AddTileData(const imgdoc2::TileBaseInfo* tile_info, imgdoc2::DataTypes datatype, imgdoc2::TileDataStorageType storage_type, const imgdoc2::IDataObjBase* data);
    //imgdoc2::dbIndex AddBlobData(imgdoc2::TileDataStorageType storage_type, const imgdoc2::IDataObjBase* data);

    //std::shared_ptr<IDbStatement> CreateInsertDataStatement(const imgdoc2::IDataObjBase* data);

    //const std::shared_ptr<imgdoc2::IHostingEnvironment>& GetHostingEnvironment() const { return this->document_->GetHostingEnvironment(); }

public:
    // no copy and no move (-> https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all )
    DocumentWrite3d() = default;
    DocumentWrite3d(const DocumentWrite3d&) = delete;             // copy constructor
    DocumentWrite3d& operator=(const DocumentWrite3d&) = delete;  // copy assignment
    DocumentWrite3d(DocumentWrite3d&&) = delete;                  // move constructor
    DocumentWrite3d& operator=(DocumentWrite3d&&) = delete;       // move assignment
};
