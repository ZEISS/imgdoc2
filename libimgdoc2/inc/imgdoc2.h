// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

/*
This file is intended to used by clients, and it contains all the public visible 
objects, functions and definitions.
So - this is the only file which is to be included by a client application.
*/

#include "loglevel.h"
#include "types.h"
#include "IEnvironment.h"
#include "TileDataStorageType.h"
#include "ICreateOptions.h"
#include "ClassFactory.h"
#include "IDocRead2d.h"
#include "IDocRead3d.h"
#include "IDocWrite2d.h"
#include "IDocWrite3d.h"
#include "IDocInfo.h"
#include "IDoc.h"
#include "TileCoordinate.h"
#include "exceptions.h"
#include "DimCoordinateQueryClause.h"
#include "TileInfoQueryClause.h"
#include "IDocumentMetadata.h"

#include "impl/BlobOutputImplementations.h"
#include "impl/DataObjectImplementations.h"
