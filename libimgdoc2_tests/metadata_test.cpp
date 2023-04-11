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
    EXPECT_DOUBLE_EQ(get<double>(item.value), 1.234);
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
    EXPECT_DOUBLE_EQ(get<double>(item.value), 1.234);
    item = metadata_reader->GetItemForPath("A/B/C", DocumentMetadataItemFlags::All);
    EXPECT_STREQ(item.name.c_str(), "C");
    EXPECT_EQ(item.type, DocumentMetadataType::Int32);
    EXPECT_EQ(get<int32_t>(item.value), 1234);
    item = metadata_reader->GetItemForPath("A/B/C/D", DocumentMetadataItemFlags::All);
    EXPECT_STREQ(item.name.c_str(), "D");
    EXPECT_EQ(item.type, DocumentMetadataType::Text);
    EXPECT_STREQ(get<string>(item.value).c_str(), "Testtext");
}

TEST(Metadata, AddMetadataItemsWithPathAndCheckIfTheyAreAdded_Scenario1)
{
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto metadata_writer = doc->GetDocumentMetadataWriter();

    auto id1 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C", DocumentMetadataType::Text, IDocumentMetadataWrite::metadata_item_variant("Testtext"));
    auto id2 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/D", DocumentMetadataType::Text, IDocumentMetadataWrite::metadata_item_variant("Testtext2"));
    auto id3 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/X/Y", DocumentMetadataType::Text, IDocumentMetadataWrite::metadata_item_variant("Testtext3"));
    EXPECT_NE(id1, id2);
    EXPECT_NE(id2, id3);
    EXPECT_NE(id1, id3);

    const auto metadata_reader = doc->GetDocumentMetadataReader();
    auto item = metadata_reader->GetItemForPath("A/B/C", DocumentMetadataItemFlags::All);
    EXPECT_STREQ(item.name.c_str(), "C");
    EXPECT_EQ(item.type, DocumentMetadataType::Text);
    EXPECT_STREQ(get<string>(item.value).c_str(), "Testtext");
    item = metadata_reader->GetItemForPath("A/B/D", DocumentMetadataItemFlags::All);
    EXPECT_STREQ(item.name.c_str(), "D");
    EXPECT_EQ(item.type, DocumentMetadataType::Text);
    EXPECT_STREQ(get<string>(item.value).c_str(), "Testtext2");
    item = metadata_reader->GetItemForPath("A/X/Y", DocumentMetadataItemFlags::All);
    EXPECT_STREQ(item.name.c_str(), "Y");
    EXPECT_EQ(item.type, DocumentMetadataType::Text);
    EXPECT_STREQ(get<string>(item.value).c_str(), "Testtext3");
}

TEST(Metadata, EnumerateItems_Scenario1)
{
    // Arrange
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto metadata_writer = doc->GetDocumentMetadataWriter();

    // we construct the following tree:
    // 
    //                 A
    //                 |
    //                 B
    //                / \
    //               C   D

    const auto id1 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C", DocumentMetadataType::Text, IDocumentMetadataWrite::metadata_item_variant("Testtext"));
    const auto id2 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/D", DocumentMetadataType::Text, IDocumentMetadataWrite::metadata_item_variant("Testtext2"));

    // Act
    vector<dbIndex> items;
    const auto metadata_reader = doc->GetDocumentMetadataReader();
    metadata_reader->EnumerateItems(
        nullopt,
        true,
        DocumentMetadataItemFlags::All,
        [&items](const auto pk, const auto item) { items.push_back(pk); return true; });

    // Assert
    EXPECT_EQ(items.size(), 4); // so, we expect 4 items, the root node and the 3 leaf nodes
    EXPECT_THAT(items, Contains(id1));
    EXPECT_THAT(items, Contains(id2));
    EXPECT_EQ(std::unique(items.begin(), items.end()), items.end());    // check that there are no duplicates, a condition which obviously must be fulfilled
}

TEST(Metadata, EnumerateItems_Scenario2)
{
    // Arrange
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto metadata_writer = doc->GetDocumentMetadataWriter();

    // we construct the following tree:
    // 
    //                 A
    //                 |
    //                 B
    //                / \
    //               C   D

    const auto id_item_b = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B", DocumentMetadataType::Null, std::monostate());
    const auto id1 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C", DocumentMetadataType::Text, IDocumentMetadataWrite::metadata_item_variant("Testtext"));
    const auto id2 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/D", DocumentMetadataType::Text, IDocumentMetadataWrite::metadata_item_variant("Testtext2"));

    // Act
    vector<dbIndex> items;
    const auto metadata_reader = doc->GetDocumentMetadataReader();
    metadata_reader->EnumerateItems(
        id_item_b,
        false,
        DocumentMetadataItemFlags::All,
        [&items](const auto pk, const auto item) { items.push_back(pk); return true; });

    // Assert
    EXPECT_EQ(items.size(), 2); // so, we expect 4 items, the root node and the 3 leaf nodes
    EXPECT_THAT(items, UnorderedElementsAre(id1, id2));
    EXPECT_EQ(std::unique(items.begin(), items.end()), items.end());    // check that there are no duplicates, a condition which obviously must be fulfilled
}
