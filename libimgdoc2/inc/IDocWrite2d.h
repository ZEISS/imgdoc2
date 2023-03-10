// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include "TileBaseInfo.h"
#include "DataTypes.h"
#include "IDataObj.h"
#include "IDatabaseTransaction.h"

namespace imgdoc2
{
    /// This interface is providing write access to a 2D-document.
    class IDocWrite2d : public imgdoc2::IDatabaseTransaction
    {
    public:
        /// Adds a tile to the document, and if successful, return its primary key.
        /// \param  coord        The coordinate.
        /// \param  info         The logical position information.
        /// \param  tileInfo     Information describing the tile.
        /// \param  datatype     The datatype.
        /// \param  storage_type Type of the storage.
        /// \param  data         The data.
        /// \returns If successful, the primary key of the newly added tile.
        virtual imgdoc2::dbIndex AddTile(
            const imgdoc2::ITileCoordinate* coord,
            const imgdoc2::LogicalPositionInfo* info,
            const imgdoc2::TileBaseInfo* tileInfo,
            imgdoc2::DataTypes datatype,
            imgdoc2::TileDataStorageType storage_type,
            const imgdoc2::IDataObjBase* data) = 0;

        ~IDocWrite2d() override = default;
    public:
        // no copy and no move (-> https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all )
        IDocWrite2d() = default;
        IDocWrite2d(const IDocWrite2d&) = delete;             // copy constructor
        IDocWrite2d& operator=(const IDocWrite2d&) = delete;  // copy assignment
        IDocWrite2d(IDocWrite2d&&) = delete;                  // move constructor
        IDocWrite2d& operator=(IDocWrite2d&&) = delete;       // move assignment
    };
}
