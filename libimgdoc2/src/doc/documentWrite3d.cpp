// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <sstream>
#include <vector> 
#include <gsl/gsl>
#include "documentWrite3d.h"
#include "transactionHelper.h"

using namespace std;
using namespace imgdoc2;

/*virtual*/imgdoc2::dbIndex DocumentWrite3d::AddBrick(
            const imgdoc2::ITileCoordinate* coord,
            const imgdoc2::LogicalPositionInfo3D* info,
            const imgdoc2::BrickBaseInfo* brickInfo,
            imgdoc2::DataTypes datatype,
            imgdoc2::TileDataStorageType storage_type,
            const imgdoc2::IDataObjBase* data)
{
    throw logic_error("The method or operation is not implemented.");
}

/*virtual*/void DocumentWrite3d::BeginTransaction()
{
    throw logic_error("The method or operation is not implemented.");
}

/*virtual*/void DocumentWrite3d::CommitTransaction()
{
    throw logic_error("The method or operation is not implemented.");
}

/*virtual*/void DocumentWrite3d::RollbackTransaction()
{
    throw logic_error("The method or operation is not implemented.");
}
