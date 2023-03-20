// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <memory>
#include "document.h"
#include "documentRead2d.h"
#include "documentWrite2d.h"
#include "documentRead3d.h"
#include "documentWrite3d.h"

using namespace std;
using namespace imgdoc2;

/*virtual*/std::shared_ptr<imgdoc2::IDocWrite2d> Document::GetWriter2d()
{
    if (this->IsDocument2d())
    {
        return std::make_shared<DocumentWrite2d>(shared_from_this());
    }

    return {};
}

/*virtual*/std::shared_ptr<imgdoc2::IDocRead2d> Document::GetReader2d()
{
    if (this->IsDocument2d())
    {
        return std::make_shared<DocumentRead2d>(shared_from_this());
    }

    return {};
}

/*virtual*/std::shared_ptr<imgdoc2::IDocWrite3d> Document::GetWriter3d()
{
    if (this->IsDocument3d())
    {
        return std::make_shared<DocumentWrite3d>(shared_from_this());
    }

    return {};
}

/*virtual*/std::shared_ptr<imgdoc2::IDocRead3d> Document::GetReader3d()
{
    if (this->IsDocument3d())
    {
        return std::make_shared<DocumentRead3d>(shared_from_this());
    }

    return {};
}
