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

TEST(Metadata, AddMetadataItemsAndCheckIfTheyAreAdded_Scenario1)
{
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto metadata_writer = doc->GetDocumentMetadataWriter();
    auto pk_node1 = metadata_writer->UpdateOrCreateItem(
        nullopt,
        true,
        "Node1",
        DocumentMetadataType::Default,
        IDocumentMetadataWrite::metadata_item_variant(std::monostate()));
    auto pk_node1_1 = metadata_writer->UpdateOrCreateItem(
        pk_node1,
        true,
        "Node1_1",
        DocumentMetadataType::Default,
        IDocumentMetadataWrite::metadata_item_variant(1.234));
    auto pk_node1_2 = metadata_writer->UpdateOrCreateItem(
        pk_node1,
        true,
        "Node1_2",
        DocumentMetadataType::Default,
        IDocumentMetadataWrite::metadata_item_variant(1234));
    auto pk_node1_3 = metadata_writer->UpdateOrCreateItem(
        pk_node1,
        true,
        "Node1_3",
        DocumentMetadataType::Default,
        IDocumentMetadataWrite::metadata_item_variant("Testtext"));

    const auto metadata_reader = doc->GetDocumentMetadataReader();
    auto item = metadata_reader->GetItem(pk_node1, DocumentMetadataItemFlags::All);
    EXPECT_STREQ(item.name.c_str(), "Node1");
    EXPECT_EQ(item.type, DocumentMetadataType::Null);
    item = metadata_reader->GetItem(pk_node1_1, DocumentMetadataItemFlags::All);
    EXPECT_STREQ(item.name.c_str(), "Node1_1");
    EXPECT_EQ(item.type, DocumentMetadataType::Double);
    EXPECT_DOUBLE_EQ(get<double>(item.value), 1.234, 0.0000001);
    item = metadata_reader->GetItem(pk_node1_2, DocumentMetadataItemFlags::All);
    EXPECT_STREQ(item.name.c_str(), "Node1_2");
    EXPECT_EQ(item.type, DocumentMetadataType::Int32);
    EXPECT_EQ(get<int32_t>(item.value), 1234);
    item = metadata_reader->GetItem(pk_node1_3, DocumentMetadataItemFlags::All);
    EXPECT_STREQ(item.name.c_str(), "Node1_3");
    EXPECT_EQ(item.type, DocumentMetadataType::Text);
    EXPECT_STREQ(get<string>(item.value).c_str(), "Testtext");
}

TEST(Metadata, AddMetadataItemsAndCheckIfTheyAreAdded_Scenario2)
{
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto metadata_writer = doc->GetDocumentMetadataWriter();
    auto pk_node1 = metadata_writer->UpdateOrCreateItem(
        nullopt,
        true,
        "A",
        DocumentMetadataType::Default,
        IDocumentMetadataWrite::metadata_item_variant(std::monostate()));
    auto pk_node1_1 = metadata_writer->UpdateOrCreateItem(
        pk_node1,
        true,
        "B",
        DocumentMetadataType::Default,
        IDocumentMetadataWrite::metadata_item_variant(1.234));
    auto pk_node1_1_1 = metadata_writer->UpdateOrCreateItem(
        pk_node1_1,
        true,
        "C",
        DocumentMetadataType::Default,
        IDocumentMetadataWrite::metadata_item_variant(1234));
    auto pk_node1_1_1_1 = metadata_writer->UpdateOrCreateItem(
        pk_node1_1_1,
        true,
        "D",
        DocumentMetadataType::Default,
        IDocumentMetadataWrite::metadata_item_variant("Testtext"));

    const auto metadata_reader = doc->GetDocumentMetadataReader();
    auto item = metadata_reader->GetItemForPath("A", DocumentMetadataItemFlags::All);
    EXPECT_STREQ(item.name.c_str(), "A");
    EXPECT_EQ(item.type, DocumentMetadataType::Null);
    item = metadata_reader->GetItemForPath("A/B", DocumentMetadataItemFlags::All);
    EXPECT_STREQ(item.name.c_str(), "B");
    EXPECT_EQ(item.type, DocumentMetadataType::Double);
    EXPECT_DOUBLE_EQ(get<double>(item.value), 1.234, 0.0000001);
    item = metadata_reader->GetItemForPath("A/B/C", DocumentMetadataItemFlags::All);
    EXPECT_STREQ(item.name.c_str(), "C");
    EXPECT_EQ(item.type, DocumentMetadataType::Int32);
    EXPECT_EQ(get<int32_t>(item.value), 1234);
    item = metadata_reader->GetItemForPath("A/B/C/D", DocumentMetadataItemFlags::All);
    EXPECT_STREQ(item.name.c_str(), "D");
    EXPECT_EQ(item.type, DocumentMetadataType::Text);
    EXPECT_STREQ(get<string>(item.value).c_str(), "Testtext");
}
