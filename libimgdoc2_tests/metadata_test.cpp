// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <random>
#include "../libimgdoc2/inc/imgdoc2.h"

using namespace std;
using namespace imgdoc2;
using namespace testing;

TEST(Metadata, CheckNodeNamesWithInvalidNamesAndExpectException)
{
    auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    auto doc = ClassFactory::CreateNew(create_options.get());

    auto metadata_writer = doc->GetDocumentMetadataWriter();
    EXPECT_THROW(
        metadata_writer->UpdateOrCreateItem(
        nullopt,
        true,
        "ABC/DEF",
        DocumentMetadataType::Default,
        IDocumentMetadataWrite::metadata_item_variant(std::monostate())),
        invalid_argument_exception);
    EXPECT_THROW(
        metadata_writer->UpdateOrCreateItem(
        nullopt,
        true,
        "",
        DocumentMetadataType::Default,
        IDocumentMetadataWrite::metadata_item_variant(std::monostate())),
        invalid_argument_exception);
}

