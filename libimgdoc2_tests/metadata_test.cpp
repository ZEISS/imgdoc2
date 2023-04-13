// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <random>
#include <algorithm>
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
        DocumentMetadataType::kDefault,
        IDocumentMetadataWrite::metadata_item_variant(std::monostate())),
        invalid_argument_exception);
    EXPECT_THROW(
        metadata_writer->UpdateOrCreateItem(
        nullopt,
        true,
        "",
        DocumentMetadataType::kDefault,
        IDocumentMetadataWrite::metadata_item_variant(std::monostate())),
        invalid_argument_exception);
}

TEST(Metadata, UpdateOrCreateItemWithInvalidParentKeyAndExpectException)
{
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto metadata_writer = doc->GetDocumentMetadataWriter();
    EXPECT_THROW(
        metadata_writer->UpdateOrCreateItem(
        123,
        true,
        "ABC",
        DocumentMetadataType::kDefault,
        IDocumentMetadataWrite::metadata_item_variant(std::monostate())),
        non_existing_item_exception);
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
        DocumentMetadataType::kDefault,
        IDocumentMetadataWrite::metadata_item_variant(std::monostate()));
    auto pk_node1_1 = metadata_writer->UpdateOrCreateItem(
        pk_node1,
        true,
        "Node1_1",
        DocumentMetadataType::kDefault,
        IDocumentMetadataWrite::metadata_item_variant(1.234));
    auto pk_node1_2 = metadata_writer->UpdateOrCreateItem(
        pk_node1,
        true,
        "Node1_2",
        DocumentMetadataType::kDefault,
        IDocumentMetadataWrite::metadata_item_variant(1234));
    auto pk_node1_3 = metadata_writer->UpdateOrCreateItem(
        pk_node1,
        true,
        "Node1_3",
        DocumentMetadataType::kDefault,
        IDocumentMetadataWrite::metadata_item_variant("Testtext"));

    const auto metadata_reader = doc->GetDocumentMetadataReader();
    auto item = metadata_reader->GetItem(pk_node1, DocumentMetadataItemFlags::kAll);
    EXPECT_STREQ(item.name.c_str(), "Node1");
    EXPECT_EQ(item.type, DocumentMetadataType::kNull);
    item = metadata_reader->GetItem(pk_node1_1, DocumentMetadataItemFlags::kAll);
    EXPECT_STREQ(item.name.c_str(), "Node1_1");
    EXPECT_EQ(item.type, DocumentMetadataType::kDouble);
    EXPECT_DOUBLE_EQ(get<double>(item.value), 1.234);
    item = metadata_reader->GetItem(pk_node1_2, DocumentMetadataItemFlags::kAll);
    EXPECT_STREQ(item.name.c_str(), "Node1_2");
    EXPECT_EQ(item.type, DocumentMetadataType::kInt32);
    EXPECT_EQ(get<int32_t>(item.value), 1234);
    item = metadata_reader->GetItem(pk_node1_3, DocumentMetadataItemFlags::kAll);
    EXPECT_STREQ(item.name.c_str(), "Node1_3");
    EXPECT_EQ(item.type, DocumentMetadataType::kText);
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
        DocumentMetadataType::kDefault,
        IDocumentMetadataWrite::metadata_item_variant(std::monostate()));
    auto pk_node1_1 = metadata_writer->UpdateOrCreateItem(
        pk_node1,
        true,
        "B",
        DocumentMetadataType::kDefault,
        IDocumentMetadataWrite::metadata_item_variant(1.234));
    auto pk_node1_1_1 = metadata_writer->UpdateOrCreateItem(
        pk_node1_1,
        true,
        "C",
        DocumentMetadataType::kDefault,
        IDocumentMetadataWrite::metadata_item_variant(1234));
    metadata_writer->UpdateOrCreateItem(
        pk_node1_1_1,
        true,
        "D",
        DocumentMetadataType::kDefault,
        IDocumentMetadataWrite::metadata_item_variant("Testtext"));

    const auto metadata_reader = doc->GetDocumentMetadataReader();
    auto item = metadata_reader->GetItemForPath("A", DocumentMetadataItemFlags::kAll);
    EXPECT_STREQ(item.name.c_str(), "A");
    EXPECT_EQ(item.type, DocumentMetadataType::kNull);
    item = metadata_reader->GetItemForPath("A/B", DocumentMetadataItemFlags::kAll);
    EXPECT_STREQ(item.name.c_str(), "B");
    EXPECT_EQ(item.type, DocumentMetadataType::kDouble);
    EXPECT_DOUBLE_EQ(get<double>(item.value), 1.234);
    item = metadata_reader->GetItemForPath("A/B/C", DocumentMetadataItemFlags::kAll);
    EXPECT_STREQ(item.name.c_str(), "C");
    EXPECT_EQ(item.type, DocumentMetadataType::kInt32);
    EXPECT_EQ(get<int32_t>(item.value), 1234);
    item = metadata_reader->GetItemForPath("A/B/C/D", DocumentMetadataItemFlags::kAll);
    EXPECT_STREQ(item.name.c_str(), "D");
    EXPECT_EQ(item.type, DocumentMetadataType::kText);
    EXPECT_STREQ(get<string>(item.value).c_str(), "Testtext");
}

TEST(Metadata, AddMetadataItemsWithPathAndCheckIfTheyAreAdded_Scenario1)
{
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto metadata_writer = doc->GetDocumentMetadataWriter();

    auto id1 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext"));
    auto id2 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/D", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext2"));
    auto id3 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/X/Y", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext3"));
    EXPECT_NE(id1, id2);
    EXPECT_NE(id2, id3);
    EXPECT_NE(id1, id3);

    const auto metadata_reader = doc->GetDocumentMetadataReader();
    auto item = metadata_reader->GetItemForPath("A/B/C", DocumentMetadataItemFlags::kAll);
    EXPECT_STREQ(item.name.c_str(), "C");
    EXPECT_EQ(item.type, DocumentMetadataType::kText);
    EXPECT_STREQ(get<string>(item.value).c_str(), "Testtext");
    item = metadata_reader->GetItemForPath("A/B/D", DocumentMetadataItemFlags::kAll);
    EXPECT_STREQ(item.name.c_str(), "D");
    EXPECT_EQ(item.type, DocumentMetadataType::kText);
    EXPECT_STREQ(get<string>(item.value).c_str(), "Testtext2");
    item = metadata_reader->GetItemForPath("A/X/Y", DocumentMetadataItemFlags::kAll);
    EXPECT_STREQ(item.name.c_str(), "Y");
    EXPECT_EQ(item.type, DocumentMetadataType::kText);
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

    const auto id1 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext"));
    const auto id2 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/D", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext2"));

    // Act
    vector<dbIndex> items;
    const auto metadata_reader = doc->GetDocumentMetadataReader();
    metadata_reader->EnumerateItems(
        nullopt,
        true,
        DocumentMetadataItemFlags::kAll,
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

    const auto id_item_b = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B", DocumentMetadataType::kNull, std::monostate());
    const auto id1 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext"));
    const auto id2 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/D", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext2"));

    // Act
    vector<dbIndex> primary_keys;
    vector<DocumentMetadataItem> items;
    const auto metadata_reader = doc->GetDocumentMetadataReader();
    metadata_reader->EnumerateItems(
        id_item_b,
        false,
        DocumentMetadataItemFlags::kAll,
        [&primary_keys, &items](const auto pk, const auto item) ->bool
        {
            primary_keys.push_back(pk);
            items.push_back(item);
            return true;
        });

    // Assert
    EXPECT_EQ(primary_keys.size(), 2); // so, we expect 4 items, the root node and the 3 leaf nodes
    EXPECT_THAT(primary_keys, UnorderedElementsAre(id1, id2));
    EXPECT_EQ(std::unique(primary_keys.begin(), primary_keys.end()), primary_keys.end());    // check that there are no duplicates, a condition which obviously must be fulfilled

    auto result_item_iterator = find_if(primary_keys.begin(), primary_keys.end(), [=](const auto& pk) { return pk == id1; });
    ASSERT_NE(result_item_iterator, primary_keys.end());
    size_t index = distance(primary_keys.begin(), result_item_iterator);
    EXPECT_TRUE((items[index].flags & DocumentMetadataItemFlags::kAll) == DocumentMetadataItemFlags::kAll);
    EXPECT_STREQ(items[index].name.c_str(), "C");
    ASSERT_EQ(items[index].type, DocumentMetadataType::kText);
    EXPECT_STREQ(get<string>(items[index].value).c_str(), "Testtext");
    EXPECT_EQ(items[index].primary_key, id1);

    result_item_iterator = find_if(primary_keys.begin(), primary_keys.end(), [=](const auto& pk) { return pk == id2; });
    ASSERT_NE(result_item_iterator, primary_keys.end());
    index = distance(primary_keys.begin(), result_item_iterator);
    EXPECT_TRUE((items[index].flags & DocumentMetadataItemFlags::kAll) == DocumentMetadataItemFlags::kAll);
    EXPECT_STREQ(items[index].name.c_str(), "D");
    ASSERT_EQ(items[index].type, DocumentMetadataType::kText);
    EXPECT_STREQ(get<string>(items[index].value).c_str(), "Testtext2");
    EXPECT_EQ(items[index].primary_key, id2);
}

TEST(Metadata, EnumerateItems_Scenario3)
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
    //              / \
    //             E   F

    const auto id_item_b = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B", DocumentMetadataType::kNull, std::monostate());
    const auto id1 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext"));
    const auto id2 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/D", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext2"));
    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C/E", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext3"));
    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C/F", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext4"));

    // Act
    vector<dbIndex> primary_keys;
    vector<DocumentMetadataItem> items;
    const auto metadata_reader = doc->GetDocumentMetadataReader();
    metadata_reader->EnumerateItems(
        id_item_b,
        false,                                      // Note: we instruct "recursive=false", so we expect only the the two direct leaf nodes of 'B' - 'C' and 'D'
        DocumentMetadataItemFlags::kAll,
        [&primary_keys, &items](const auto pk, const auto item) ->bool
        {
            primary_keys.push_back(pk);
            items.push_back(item);
            return true;
        });

    // Assert
    EXPECT_EQ(primary_keys.size(), 2); // so, we expect 4 items, the root node and the 3 leaf nodes
    EXPECT_THAT(primary_keys, UnorderedElementsAre(id1, id2));
    EXPECT_EQ(std::unique(primary_keys.begin(), primary_keys.end()), primary_keys.end());    // check that there are no duplicates, a condition which obviously must be fulfilled

    auto result_item_iterator = find_if(primary_keys.begin(), primary_keys.end(), [=](const auto& pk) { return pk == id1; });
    ASSERT_NE(result_item_iterator, primary_keys.end());
    size_t index = distance(primary_keys.begin(), result_item_iterator);
    EXPECT_TRUE((items[index].flags & DocumentMetadataItemFlags::kAll) == DocumentMetadataItemFlags::kAll);
    EXPECT_STREQ(items[index].name.c_str(), "C");
    ASSERT_EQ(items[index].type, DocumentMetadataType::kText);
    EXPECT_STREQ(get<string>(items[index].value).c_str(), "Testtext");
    EXPECT_EQ(items[index].primary_key, id1);

    result_item_iterator = find_if(primary_keys.begin(), primary_keys.end(), [=](const auto& pk) { return pk == id2; });
    ASSERT_NE(result_item_iterator, primary_keys.end());
    index = distance(primary_keys.begin(), result_item_iterator);
    EXPECT_TRUE((items[index].flags & DocumentMetadataItemFlags::kAll) == DocumentMetadataItemFlags::kAll);
    EXPECT_STREQ(items[index].name.c_str(), "D");
    ASSERT_EQ(items[index].type, DocumentMetadataType::kText);
    EXPECT_STREQ(get<string>(items[index].value).c_str(), "Testtext2");
    EXPECT_EQ(items[index].primary_key, id2);
}

TEST(Metadata, EnumerateItems_Scenario4)
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
    //              / \
    //             E   F

    const auto id_item_b = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B", DocumentMetadataType::kNull, std::monostate());
    const auto id1 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext"));
    const auto id2 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/D", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext2"));
    const auto id3 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C/E", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext3"));
    const auto id4 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C/F", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext4"));

    // Act
    vector<dbIndex> primary_keys;
    vector<DocumentMetadataItem> items;
    const auto metadata_reader = doc->GetDocumentMetadataReader();
    metadata_reader->EnumerateItems(
        id_item_b,
        true,                                      // Note: we instruct "recursive=true", so we expect to have all 4 leaf nodes of 'B' : 'C', 'D', 'E' and 'F'
        DocumentMetadataItemFlags::kAll,
        [&primary_keys, &items](const auto pk, const auto item) ->bool
        {
            primary_keys.push_back(pk);
            items.push_back(item);
            return true;
        });

    // Assert
    EXPECT_EQ(primary_keys.size(), 4); // so, we expect 4 items, the root node and the 3 leaf nodes
    EXPECT_THAT(primary_keys, UnorderedElementsAre(id1, id2, id3, id4));
    EXPECT_EQ(std::unique(primary_keys.begin(), primary_keys.end()), primary_keys.end());    // check that there are no duplicates, a condition which obviously must be fulfilled

    auto result_item_iterator = find_if(primary_keys.begin(), primary_keys.end(), [=](const auto& pk) { return pk == id1; });
    ASSERT_NE(result_item_iterator, primary_keys.end());
    size_t index = distance(primary_keys.begin(), result_item_iterator);
    EXPECT_TRUE((items[index].flags & DocumentMetadataItemFlags::kAll) == DocumentMetadataItemFlags::kAll);
    EXPECT_STREQ(items[index].name.c_str(), "C");
    ASSERT_EQ(items[index].type, DocumentMetadataType::kText);
    EXPECT_STREQ(get<string>(items[index].value).c_str(), "Testtext");
    EXPECT_EQ(items[index].primary_key, id1);

    result_item_iterator = find_if(primary_keys.begin(), primary_keys.end(), [=](const auto& pk) { return pk == id2; });
    ASSERT_NE(result_item_iterator, primary_keys.end());
    index = distance(primary_keys.begin(), result_item_iterator);
    EXPECT_TRUE((items[index].flags & DocumentMetadataItemFlags::kAll) == DocumentMetadataItemFlags::kAll);
    EXPECT_STREQ(items[index].name.c_str(), "D");
    ASSERT_EQ(items[index].type, DocumentMetadataType::kText);
    EXPECT_STREQ(get<string>(items[index].value).c_str(), "Testtext2");
    EXPECT_EQ(items[index].primary_key, id2);

    result_item_iterator = find_if(primary_keys.begin(), primary_keys.end(), [=](const auto& pk) { return pk == id3; });
    ASSERT_NE(result_item_iterator, primary_keys.end());
    index = distance(primary_keys.begin(), result_item_iterator);
    EXPECT_TRUE((items[index].flags & DocumentMetadataItemFlags::kAll) == DocumentMetadataItemFlags::kAll);
    EXPECT_STREQ(items[index].name.c_str(), "E");
    ASSERT_EQ(items[index].type, DocumentMetadataType::kText);
    EXPECT_STREQ(get<string>(items[index].value).c_str(), "Testtext3");
    EXPECT_EQ(items[index].primary_key, id3);

    result_item_iterator = find_if(primary_keys.begin(), primary_keys.end(), [=](const auto& pk) { return pk == id4; });
    ASSERT_NE(result_item_iterator, primary_keys.end());
    index = distance(primary_keys.begin(), result_item_iterator);
    EXPECT_TRUE((items[index].flags & DocumentMetadataItemFlags::kAll) == DocumentMetadataItemFlags::kAll);
    EXPECT_STREQ(items[index].name.c_str(), "F");
    ASSERT_EQ(items[index].type, DocumentMetadataType::kText);
    EXPECT_STREQ(get<string>(items[index].value).c_str(), "Testtext4");
    EXPECT_EQ(items[index].primary_key, id4);
}

TEST(Metadata, EnumerateItemsForPath_Scenario1)
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

    const auto id1 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext"));
    const auto id2 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/D", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext2"));

    // Act
    vector<dbIndex> items;
    const auto metadata_reader = doc->GetDocumentMetadataReader();
    metadata_reader->EnumerateItemsForPath(
        "",
        true,
        DocumentMetadataItemFlags::kAll,
        [&items](const auto pk, const auto item) { items.push_back(pk); return true; });

    // Assert
    EXPECT_EQ(items.size(), 4); // so, we expect 4 items, the root node and the 3 leaf nodes
    EXPECT_THAT(items, Contains(id1));
    EXPECT_THAT(items, Contains(id2));
    EXPECT_EQ(std::unique(items.begin(), items.end()), items.end());    // check that there are no duplicates, a condition which obviously must be fulfilled
}

TEST(Metadata, EnumerateItemsForPath_Scenario2)
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

    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B", DocumentMetadataType::kNull, std::monostate());
    const auto id1 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext"));
    const auto id2 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/D", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext2"));

    // Act
    vector<dbIndex> primary_keys;
    vector<DocumentMetadataItem> items;
    const auto metadata_reader = doc->GetDocumentMetadataReader();
    metadata_reader->EnumerateItemsForPath(
        "A/B",
        false,
        DocumentMetadataItemFlags::kAll,
        [&primary_keys, &items](const auto pk, const auto item) ->bool
        {
            primary_keys.push_back(pk);
            items.push_back(item);
            return true;
        });

    // Assert
    EXPECT_EQ(primary_keys.size(), 2); // so, we expect 4 items, the root node and the 3 leaf nodes
    EXPECT_THAT(primary_keys, UnorderedElementsAre(id1, id2));
    EXPECT_EQ(std::unique(primary_keys.begin(), primary_keys.end()), primary_keys.end());    // check that there are no duplicates, a condition which obviously must be fulfilled

    auto result_item_iterator = find_if(primary_keys.begin(), primary_keys.end(), [=](const auto& pk) { return pk == id1; });
    ASSERT_NE(result_item_iterator, primary_keys.end());
    size_t index = distance(primary_keys.begin(), result_item_iterator);
    EXPECT_TRUE((items[index].flags & DocumentMetadataItemFlags::kAll) == DocumentMetadataItemFlags::kAll);
    EXPECT_STREQ(items[index].name.c_str(), "C");
    ASSERT_EQ(items[index].type, DocumentMetadataType::kText);
    EXPECT_STREQ(get<string>(items[index].value).c_str(), "Testtext");
    EXPECT_EQ(items[index].primary_key, id1);

    result_item_iterator = find_if(primary_keys.begin(), primary_keys.end(), [=](const auto& pk) { return pk == id2; });
    ASSERT_NE(result_item_iterator, primary_keys.end());
    index = distance(primary_keys.begin(), result_item_iterator);
    EXPECT_TRUE((items[index].flags & DocumentMetadataItemFlags::kAll) == DocumentMetadataItemFlags::kAll);
    EXPECT_STREQ(items[index].name.c_str(), "D");
    ASSERT_EQ(items[index].type, DocumentMetadataType::kText);
    EXPECT_STREQ(get<string>(items[index].value).c_str(), "Testtext2");
    EXPECT_EQ(items[index].primary_key, id2);
}

TEST(Metadata, EnumerateItemsForPath_Scenario3)
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
    //              / \
    //             E   F

    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B", DocumentMetadataType::kNull, std::monostate());
    const auto id1 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext"));
    const auto id2 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/D", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext2"));
    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C/E", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext3"));
    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C/F", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext4"));

    // Act
    vector<dbIndex> primary_keys;
    vector<DocumentMetadataItem> items;
    const auto metadata_reader = doc->GetDocumentMetadataReader();
    metadata_reader->EnumerateItemsForPath(
        "A/B",
        false,                                      // Note: we instruct "recursive=false", so we expect only the the two direct leaf nodes of 'B' - 'C' and 'D'
        DocumentMetadataItemFlags::kAll,
        [&primary_keys, &items](const auto pk, const auto item) ->bool
        {
            primary_keys.push_back(pk);
            items.push_back(item);
            return true;
        });

    // Assert
    EXPECT_EQ(primary_keys.size(), 2); // so, we expect 4 items, the root node and the 3 leaf nodes
    EXPECT_THAT(primary_keys, UnorderedElementsAre(id1, id2));
    EXPECT_EQ(std::unique(primary_keys.begin(), primary_keys.end()), primary_keys.end());    // check that there are no duplicates, a condition which obviously must be fulfilled

    auto result_item_iterator = find_if(primary_keys.begin(), primary_keys.end(), [=](const auto& pk) { return pk == id1; });
    ASSERT_NE(result_item_iterator, primary_keys.end());
    size_t index = distance(primary_keys.begin(), result_item_iterator);
    EXPECT_TRUE((items[index].flags & DocumentMetadataItemFlags::kAll) == DocumentMetadataItemFlags::kAll);
    EXPECT_STREQ(items[index].name.c_str(), "C");
    ASSERT_EQ(items[index].type, DocumentMetadataType::kText);
    EXPECT_STREQ(get<string>(items[index].value).c_str(), "Testtext");
    EXPECT_EQ(items[index].primary_key, id1);

    result_item_iterator = find_if(primary_keys.begin(), primary_keys.end(), [=](const auto& pk) { return pk == id2; });
    ASSERT_NE(result_item_iterator, primary_keys.end());
    index = distance(primary_keys.begin(), result_item_iterator);
    EXPECT_TRUE((items[index].flags & DocumentMetadataItemFlags::kAll) == DocumentMetadataItemFlags::kAll);
    EXPECT_STREQ(items[index].name.c_str(), "D");
    ASSERT_EQ(items[index].type, DocumentMetadataType::kText);
    EXPECT_STREQ(get<string>(items[index].value).c_str(), "Testtext2");
    EXPECT_EQ(items[index].primary_key, id2);
}

TEST(Metadata, DeleteItem_Scenario1)
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
    //              / \
    //             E   F

    const auto id_b = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B", DocumentMetadataType::kNull, std::monostate());
    const auto id_c = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext"));
    const auto id_d = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/D", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext2"));
    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C/E", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext3"));
    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C/F", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext4"));

    // Act & Assert

    // now, try to delete the node 'C' - this should fail (or - return zero deleted nodes/row), because it has children
    auto number_of_nodes_deleted = metadata_writer->DeleteItem(id_c, false);
    EXPECT_EQ(number_of_nodes_deleted, 0);

    // now, try again, but this time, instruct the delete to be recursive - this should succeed (and remove 'C', 'E' and 'F')
    number_of_nodes_deleted = metadata_writer->DeleteItem(id_c, true);
    EXPECT_EQ(number_of_nodes_deleted, 3);

    // now, try to delete the node 'B' - this should fail (or - return zero deleted nodes/row), because it has a child ('D')
    number_of_nodes_deleted = metadata_writer->DeleteItem(id_b, false);
    EXPECT_EQ(number_of_nodes_deleted, 0);

    // next, delete the node 'D'
    number_of_nodes_deleted = metadata_writer->DeleteItem(id_d, false);
    EXPECT_EQ(number_of_nodes_deleted, 1);

    // so, now 'B' has no children anymore, so we can delete it
    number_of_nodes_deleted = metadata_writer->DeleteItem(id_b, false);
    EXPECT_EQ(number_of_nodes_deleted, 1);
}

TEST(Metadata, DeleteItemDeleteRoot)
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
    //              / \
    //             E   F

    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B", DocumentMetadataType::kNull, std::monostate());
    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext"));
    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/D", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext2"));
    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C/E", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext3"));
    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C/F", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext4"));

    // Act & Assert

    // first, try to delete the "root" with "recursive=false" - this should always to nothing
    auto number_of_nodes_deleted = metadata_writer->DeleteItem(nullopt, false);
    EXPECT_EQ(number_of_nodes_deleted, 0);

    // then - try to delete the "root" with "recursive=true" - this should delete all nodes
    number_of_nodes_deleted = metadata_writer->DeleteItem(nullopt, true);
    // we expect that all nodes have been deleted, which are 6 altogether (A, B, C, D, E, F)
    EXPECT_EQ(number_of_nodes_deleted, 6);
}

TEST(Metadata, DeleteItemForPath_Scenario1)
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
    //              / \
    //             E   F

    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B", DocumentMetadataType::kNull, std::monostate());
    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext"));
    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/D", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext2"));
    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C/E", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext3"));
    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C/F", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext4"));

    // Act & Assert

    // now, try to delete the node 'C' - this should fail (or - return zero deleted nodes/row), because it has children
    auto number_of_nodes_deleted = metadata_writer->DeleteItemForPath("A/B/C", false);
    EXPECT_EQ(number_of_nodes_deleted, 0);

    // now, try again, but this time, instruct the delete to be recursive - this should succeed (and remove 'C', 'E' and 'F')
    number_of_nodes_deleted = metadata_writer->DeleteItemForPath("A/B/C", true);
    EXPECT_EQ(number_of_nodes_deleted, 3);

    // now, try to delete the node 'B' - this should fail (or - return zero deleted nodes/row), because it has a child ('D')
    number_of_nodes_deleted = metadata_writer->DeleteItemForPath("A/B", false);
    EXPECT_EQ(number_of_nodes_deleted, 0);

    // next, delete the node 'D'
    number_of_nodes_deleted = metadata_writer->DeleteItemForPath("A/B/D", false);
    EXPECT_EQ(number_of_nodes_deleted, 1);

    // so, now 'B' has no children anymore, so we can delete it
    number_of_nodes_deleted = metadata_writer->DeleteItemForPath("A/B", false);
    EXPECT_EQ(number_of_nodes_deleted, 1);
}

TEST(Metadata, DeleteItemForPathDeleteRoot)
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
    //              / \
    //             E   F

    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B", DocumentMetadataType::kNull, std::monostate());
    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext"));
    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/D", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext2"));
    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C/E", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext3"));
    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C/F", DocumentMetadataType::kText, IDocumentMetadataWrite::metadata_item_variant("Testtext4"));

    // Act & Assert

    // first, try to delete the "root" with "recursive=false" - this should always to nothing
    auto number_of_nodes_deleted = metadata_writer->DeleteItemForPath("", false);
    EXPECT_EQ(number_of_nodes_deleted, 0);

    // then - try to delete the "root" with "recursive=true" - this should delete all nodes
    number_of_nodes_deleted = metadata_writer->DeleteItemForPath("", true);
    // we expect that all nodes have been deleted, which are 6 altogether (A, B, C, D, E, F)
    EXPECT_EQ(number_of_nodes_deleted, 6);
}

TEST(Metadata, ConstructDeepMetadataHierarchyAndDeleteAllItems)
{
    // Arrange
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto metadata_writer = doc->GetDocumentMetadataWriter();

    metadata_writer->UpdateOrCreateItemForPath(
        true,
        true,
        "A/B/C/D/E/F/G/H/I/J/K/L/M/N/O/P/Q/R/S/T/U/V/W/X/Y/Z",
        DocumentMetadataType::kText,
        IDocumentMetadataWrite::metadata_item_variant("Testtext"));

    // Act
    const auto number_of_nodes_deleted = metadata_writer->DeleteItemForPath("", true);

    // Assert

    // we expect that all nodes have been deleted, which are 26 altogether (A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z)
    EXPECT_EQ(number_of_nodes_deleted, 26);
}

TEST(Metadata, InvalidPathWithGetItem)
{
    // Arrange
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto metadata_reader = doc->GetDocumentMetadataReader();

    EXPECT_THROW(metadata_reader->GetItemForPath("/test", DocumentMetadataItemFlags::kAll), invalid_path_exception);
    EXPECT_THROW(metadata_reader->GetItemForPath("test//", DocumentMetadataItemFlags::kAll), invalid_path_exception);
    EXPECT_THROW(metadata_reader->GetItemForPath("test/", DocumentMetadataItemFlags::kAll), invalid_path_exception);
    EXPECT_THROW(metadata_reader->GetItemForPath("test//abc", DocumentMetadataItemFlags::kAll), invalid_path_exception);
    EXPECT_THROW(metadata_reader->GetItemForPath("abc/def//ghi", DocumentMetadataItemFlags::kAll), invalid_path_exception);
}

TEST(Metadata, CallGetItemForNonExistingItemAndExpectError)
{
    // Arrange
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto metadata_reader = doc->GetDocumentMetadataReader();
    const auto metadata_writer = doc->GetDocumentMetadataWriter();

    const auto key = metadata_writer->UpdateOrCreateItemForPath(
        true,
        true,
        "AAAABBBB",
        DocumentMetadataType::kText,
        IDocumentMetadataWrite::metadata_item_variant("Testtext"));

    const auto invalid_key = key + 1;

    EXPECT_THROW(metadata_reader->GetItem(invalid_key, DocumentMetadataItemFlags::kAll), non_existing_item_exception);
}

TEST(Metadata, CallGetItemForPathForNonExistingItemAndExpectError)
{
    // Arrange
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto metadata_reader = doc->GetDocumentMetadataReader();
    const auto metadata_writer = doc->GetDocumentMetadataWriter();

    metadata_writer->UpdateOrCreateItemForPath(
        true,
        true,
        "AAAABBBB",
        DocumentMetadataType::kText,
        IDocumentMetadataWrite::metadata_item_variant("Testtext"));

    // Act & Assert
    EXPECT_THROW(metadata_reader->GetItemForPath("AAAABBBB/QQQ", DocumentMetadataItemFlags::kAll), invalid_path_exception);
    EXPECT_THROW(metadata_reader->GetItemForPath("AAAABBBB//QQQ", DocumentMetadataItemFlags::kAll), invalid_path_exception);
    EXPECT_THROW(metadata_reader->GetItemForPath("AAAABBB", DocumentMetadataItemFlags::kAll), invalid_path_exception);
}

TEST(Metadata, CallEnumerateItemsForPathForNonExistingItemAndExpectError)
{
    // Arrange
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto metadata_reader = doc->GetDocumentMetadataReader();
    const auto metadata_writer = doc->GetDocumentMetadataWriter();

    metadata_writer->UpdateOrCreateItemForPath(
        true,
        true,
        "AAAABBBB",
        DocumentMetadataType::kText,
        IDocumentMetadataWrite::metadata_item_variant("Testtext"));

    // Act & Assert
    EXPECT_THROW(metadata_reader->EnumerateItemsForPath("Testtext2", true, DocumentMetadataItemFlags::kAll, [](auto x, auto y) {return true; }), invalid_path_exception);
}

TEST(Metadata, GetItemWithFullPathCheckResult_Scenario1)
{
    // Arrange
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto metadata_reader = doc->GetDocumentMetadataReader();
    const auto metadata_writer = doc->GetDocumentMetadataWriter();

    const auto pk = metadata_writer->UpdateOrCreateItemForPath(
        true,
        true,
        "A/B/C/D/E/F/G/H/I/J/K/L/M/N/O/P/Q/R/S/T/U/V/W/X/Y/Z",
        DocumentMetadataType::kText,
        IDocumentMetadataWrite::metadata_item_variant("Testtext"));

    auto item = metadata_reader->GetItem(pk, DocumentMetadataItemFlags::kAllWithCompletePath);
    EXPECT_TRUE((item.flags & DocumentMetadataItemFlags::kCompletePath) == DocumentMetadataItemFlags::kCompletePath);
    EXPECT_STREQ(item.complete_path.c_str(), "A/B/C/D/E/F/G/H/I/J/K/L/M/N/O/P/Q/R/S/T/U/V/W/X/Y/Z");

    auto item2 = metadata_reader->GetItem(pk, DocumentMetadataItemFlags::kCompletePath);
    EXPECT_TRUE((item2.flags & DocumentMetadataItemFlags::kCompletePath) == DocumentMetadataItemFlags::kCompletePath);
    EXPECT_STREQ(item2.complete_path.c_str(), "A/B/C/D/E/F/G/H/I/J/K/L/M/N/O/P/Q/R/S/T/U/V/W/X/Y/Z");
}

TEST(Metadata, GetItemWithFullPathCheckResult_Scenario2)
{
    // Arrange
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto metadata_reader = doc->GetDocumentMetadataReader();
    const auto metadata_writer = doc->GetDocumentMetadataWriter();

    const auto pk1 = metadata_writer->UpdateOrCreateItemForPath(
        true,
        true,
        "A/B/C/D/E/F/G/H/I/J/K/L/M/N/O/P/Q/R/S/T/U/V/W/X/Y/Z",
        DocumentMetadataType::kText,
        IDocumentMetadataWrite::metadata_item_variant("Testtext"));

    const auto pk2 = metadata_writer->UpdateOrCreateItemForPath(
        true,
        true,
        "A/B/C/D/E/F/G/H/I/J/K/L/M/N/O/P/Q/R/S/T/U/V/W/X/YY/ZZ",
        DocumentMetadataType::kText,
        IDocumentMetadataWrite::metadata_item_variant("Testtext2"));

    auto item = metadata_reader->GetItem(pk1, DocumentMetadataItemFlags::kCompletePath);
    EXPECT_TRUE((item.flags & DocumentMetadataItemFlags::kCompletePath) == DocumentMetadataItemFlags::kCompletePath);
    EXPECT_STREQ(item.complete_path.c_str(), "A/B/C/D/E/F/G/H/I/J/K/L/M/N/O/P/Q/R/S/T/U/V/W/X/Y/Z");

    auto item2 = metadata_reader->GetItem(pk2, DocumentMetadataItemFlags::kCompletePath);
    EXPECT_TRUE((item2.flags & DocumentMetadataItemFlags::kCompletePath) == DocumentMetadataItemFlags::kCompletePath);
    EXPECT_STREQ(item2.complete_path.c_str(), "A/B/C/D/E/F/G/H/I/J/K/L/M/N/O/P/Q/R/S/T/U/V/W/X/YY/ZZ");
}

TEST(Metadata, GetItemForNonExistingItemTestAllFlags)
{
    // Arrange
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto metadata_reader = doc->GetDocumentMetadataReader();
    const auto metadata_writer = doc->GetDocumentMetadataWriter();

    const auto key = metadata_writer->UpdateOrCreateItemForPath(
        true,
        true,
        "AAAABBBB",
        DocumentMetadataType::kText,
        IDocumentMetadataWrite::metadata_item_variant("Testtext"));

    const auto invalid_key = key + 1;

    EXPECT_THROW(metadata_reader->GetItem(invalid_key, DocumentMetadataItemFlags::kAll), non_existing_item_exception);
    EXPECT_THROW(metadata_reader->GetItem(invalid_key, DocumentMetadataItemFlags::kAllWithCompletePath), non_existing_item_exception);
    EXPECT_THROW(metadata_reader->GetItem(invalid_key, DocumentMetadataItemFlags::None), non_existing_item_exception);
    EXPECT_THROW(metadata_reader->GetItem(invalid_key, DocumentMetadataItemFlags::kPrimaryKeyValid), non_existing_item_exception);
    EXPECT_THROW(metadata_reader->GetItem(invalid_key, DocumentMetadataItemFlags::kNameValid), non_existing_item_exception);
    EXPECT_THROW(metadata_reader->GetItem(invalid_key, DocumentMetadataItemFlags::kDocumentMetadataTypeAndValueValid), non_existing_item_exception);
    EXPECT_THROW(metadata_reader->GetItem(invalid_key, DocumentMetadataItemFlags::kCompletePath), non_existing_item_exception);
    EXPECT_THROW(metadata_reader->GetItem(invalid_key, DocumentMetadataItemFlags::kPrimaryKeyValid | DocumentMetadataItemFlags::kNameValid), non_existing_item_exception);
    EXPECT_THROW(metadata_reader->GetItem(invalid_key, DocumentMetadataItemFlags::kPrimaryKeyValid | DocumentMetadataItemFlags::kNameValid | DocumentMetadataItemFlags::kDocumentMetadataTypeAndValueValid), non_existing_item_exception);
}

TEST(Metadata, EnumerateItemsFullPathCheckResult_Scenario1)
{
    // Arrange
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto metadata_reader = doc->GetDocumentMetadataReader();
    const auto metadata_writer = doc->GetDocumentMetadataWriter();

    metadata_writer->UpdateOrCreateItemForPath(
        true,
        true,
        "A/B/C/D/E/F/G/H/I/J/K/L/M/N/O/P/Q/R/S/T/U/V/W/X/Y/Z",
        DocumentMetadataType::kText,
        IDocumentMetadataWrite::metadata_item_variant("Testtext"));

    vector<DocumentMetadataItem> results;
    metadata_reader->EnumerateItems(nullopt, false, DocumentMetadataItemFlags::kAllWithCompletePath,
        [&results](const auto pk, const auto item)
        {
            results.push_back(item);
            return true;
        });

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(results.at(0).flags & DocumentMetadataItemFlags::kCompletePath, DocumentMetadataItemFlags::kCompletePath);
    EXPECT_STREQ(results.at(0).complete_path.c_str(), "A");

    results.clear();
    metadata_reader->EnumerateItems(nullopt, true, DocumentMetadataItemFlags::kAllWithCompletePath,
        [&results](const auto pk, const auto item)
        {
            results.push_back(item);
            return true;
        });

    EXPECT_EQ(results.size(), 26);
    bool all_true = all_of(
        results.begin(),
        results.end(),
        [](const DocumentMetadataItem& i)
        {
            return (i.flags & DocumentMetadataItemFlags::kCompletePath) == DocumentMetadataItemFlags::kCompletePath;
        });
    EXPECT_TRUE(all_true);

    all_true = all_of(
        results.begin(),
        results.end(),
        [](const DocumentMetadataItem& i)
        {
            if (i.name == "A")
            {
                return i.complete_path == "A";
            }
            else if (i.name == "B")
            {
                return i.complete_path == "A/B";
            }
            else if (i.name == "C")
            {
                return i.complete_path == "A/B/C";
            }
            else if (i.name == "D")
            {
                return i.complete_path == "A/B/C/D";
            }
            else if (i.name == "E")
            {
                return i.complete_path == "A/B/C/D/E";
            }
            else if (i.name == "F")
            {
                return i.complete_path == "A/B/C/D/E/F";
            }
            else if (i.name == "G")
            {
                return i.complete_path == "A/B/C/D/E/F/G";
            }
            else if (i.name == "H")
            {
                return i.complete_path == "A/B/C/D/E/F/G/H";
            }
            else if (i.name == "I")
            {
                return i.complete_path == "A/B/C/D/E/F/G/H/I";
            }
            else if (i.name == "J")
            {
                return i.complete_path == "A/B/C/D/E/F/G/H/I/J";
            }
            else if (i.name == "K")
            {
                return i.complete_path == "A/B/C/D/E/F/G/H/I/J/K";
            }
            else if (i.name == "L")
            {
                return i.complete_path == "A/B/C/D/E/F/G/H/I/J/K/L";
            }
            else if (i.name == "M")
            {
                return i.complete_path == "A/B/C/D/E/F/G/H/I/J/K/L/M";
            }
            else if (i.name == "N")
            {
                return i.complete_path == "A/B/C/D/E/F/G/H/I/J/K/L/M/N";
            }
            else if (i.name == "O")
            {
                return i.complete_path == "A/B/C/D/E/F/G/H/I/J/K/L/M/N/O";
            }
            else if (i.name == "P")
            {
                return i.complete_path == "A/B/C/D/E/F/G/H/I/J/K/L/M/N/O/P";
            }
            else if (i.name == "Q")
            {
                return i.complete_path == "A/B/C/D/E/F/G/H/I/J/K/L/M/N/O/P/Q";
            }
            else if (i.name == "R")
            {
                return i.complete_path == "A/B/C/D/E/F/G/H/I/J/K/L/M/N/O/P/Q/R";
            }
            else if (i.name == "S")
            {
                return i.complete_path == "A/B/C/D/E/F/G/H/I/J/K/L/M/N/O/P/Q/R/S";
            }
            else if (i.name == "T")
            {
                return i.complete_path == "A/B/C/D/E/F/G/H/I/J/K/L/M/N/O/P/Q/R/S/T";
            }
            else if (i.name == "U")
            {
                return i.complete_path == "A/B/C/D/E/F/G/H/I/J/K/L/M/N/O/P/Q/R/S/T/U";
            }
            else if (i.name == "V")
            {
                return i.complete_path == "A/B/C/D/E/F/G/H/I/J/K/L/M/N/O/P/Q/R/S/T/U/V";
            }
            else if (i.name == "W")
            {
                return i.complete_path == "A/B/C/D/E/F/G/H/I/J/K/L/M/N/O/P/Q/R/S/T/U/V/W";
            }
            else if (i.name == "X")
            {
                return i.complete_path == "A/B/C/D/E/F/G/H/I/J/K/L/M/N/O/P/Q/R/S/T/U/V/W/X";
            }
            else if (i.name == "Y")
            {
                return i.complete_path == "A/B/C/D/E/F/G/H/I/J/K/L/M/N/O/P/Q/R/S/T/U/V/W/X/Y";
            }
            else if (i.name == "Z")
            {
                return i.complete_path == "A/B/C/D/E/F/G/H/I/J/K/L/M/N/O/P/Q/R/S/T/U/V/W/X/Y/Z";
            }

            return false;
        });
    EXPECT_TRUE(all_true);
}

TEST(Metadata, EnumerateItemsFullPathCheckResult_Scenario2)
{
    // Arrange
    const auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    const auto doc = ClassFactory::CreateNew(create_options.get());
    const auto metadata_writer = doc->GetDocumentMetadataWriter();
    const auto metadata_reader = doc->GetDocumentMetadataReader();

    // we construct the following tree:
    // 
    //                 A
    //                 |
    //                 B
    //                / \
    //               C   D
    //              / \
    //             E   F

    const auto id_item_a = metadata_writer->UpdateOrCreateItem(nullopt, true, "A", DocumentMetadataType::kNull, std::monostate());
    const auto id_item_b = metadata_writer->UpdateOrCreateItem(id_item_a, true, "B", DocumentMetadataType::kNull, std::monostate());
    const auto id_item_c = metadata_writer->UpdateOrCreateItem(id_item_b, true, "C", DocumentMetadataType::kNull, std::monostate());
    metadata_writer->UpdateOrCreateItem(id_item_b, true, "D", DocumentMetadataType::kNull, std::monostate());
    metadata_writer->UpdateOrCreateItem(id_item_c, true, "E", DocumentMetadataType::kNull, std::monostate());
    metadata_writer->UpdateOrCreateItem(id_item_c, true, "F", DocumentMetadataType::kNull, std::monostate());

    // Act
    vector<DocumentMetadataItem> results;

    // query for all items with complete path
    metadata_reader->EnumerateItems(
        nullopt,
        true,
        DocumentMetadataItemFlags::kAllWithCompletePath,
        [&results](const auto pk, const auto item)
        {
            results.push_back(item);
            return true;
        });

    // Assert
    EXPECT_EQ(results.size(), 6);
    bool all_true = all_of(
        results.begin(),
        results.end(),
        [](const DocumentMetadataItem& i)
        {
            return (i.flags & (DocumentMetadataItemFlags::kCompletePath | DocumentMetadataItemFlags::kNameValid)) == (DocumentMetadataItemFlags::kCompletePath | DocumentMetadataItemFlags::kNameValid);
        });
    EXPECT_TRUE(all_true);
    all_true = all_of(
        results.begin(),
        results.end(),
        [](const DocumentMetadataItem& i)
        {
            if (i.name == "A")
            {
                return i.complete_path == "A";
            }
            else if (i.name == "B")
            {
                return i.complete_path == "A/B";
            }
            else if (i.name == "C")
            {
                return i.complete_path == "A/B/C";
            }
            else if (i.name == "D")
            {
                return i.complete_path == "A/B/D";
            }
            else if (i.name == "E")
            {
                return i.complete_path == "A/B/C/E";
            }
            else if (i.name == "F")
            {
                return i.complete_path == "A/B/C/F";
            }

            return false;
        });
    EXPECT_TRUE(all_true);

    // Act

    // query for direct and indirect children of C
    results.clear();
    metadata_reader->EnumerateItems(
        id_item_c,
        true,
        DocumentMetadataItemFlags::kAllWithCompletePath,
        [&results](const auto pk, const auto item)
        {
            results.push_back(item);
            return true;
        });

    // Assert
    EXPECT_EQ(results.size(), 2);
    all_true = all_of(
        results.begin(),
        results.end(),
        [](const DocumentMetadataItem& i)
        {
            return (i.flags & (DocumentMetadataItemFlags::kCompletePath | DocumentMetadataItemFlags::kNameValid)) == (DocumentMetadataItemFlags::kCompletePath | DocumentMetadataItemFlags::kNameValid);
        });
    EXPECT_TRUE(all_true);
    all_true = all_of(
        results.begin(),
        results.end(),
        [](const DocumentMetadataItem& i)
        {
            if (i.name == "E")
            {
                return i.complete_path == "A/B/C/E";
            }
            else if (i.name == "F")
            {
                return i.complete_path == "A/B/C/F";
            }

            return false;
        });
    EXPECT_TRUE(all_true);

    // Act

    // query for direct children of B
    results.clear();
    metadata_reader->EnumerateItems(
        id_item_b,
        false,
        DocumentMetadataItemFlags::kAllWithCompletePath,
        [&results](const auto pk, const auto item)
        {
            results.push_back(item);
            return true;
        });

    // Assert
    EXPECT_EQ(results.size(), 2);
    all_true = all_of(
        results.begin(),
        results.end(),
        [](const DocumentMetadataItem& i)
        {
            return (i.flags & (DocumentMetadataItemFlags::kCompletePath | DocumentMetadataItemFlags::kNameValid)) == (DocumentMetadataItemFlags::kCompletePath | DocumentMetadataItemFlags::kNameValid);
        });
    EXPECT_TRUE(all_true);
    all_true = all_of(
        results.begin(),
        results.end(),
        [](const DocumentMetadataItem& i)
        {
            if (i.name == "C")
            {
                return i.complete_path == "A/B/C";
            }
            else if (i.name == "D")
            {
                return i.complete_path == "A/B/D";
            }

            return false;
        });
    EXPECT_TRUE(all_true);

    // Act

    // query for direct and indirect children of B
    results.clear();
    metadata_reader->EnumerateItems(
        id_item_b,
        true,
        DocumentMetadataItemFlags::kAllWithCompletePath,
        [&results](const auto pk, const auto item)
        {
            results.push_back(item);
            return true;
        });

    // Assert
    EXPECT_EQ(results.size(), 4);
    all_true = all_of(
        results.begin(),
        results.end(),
        [](const DocumentMetadataItem& i)
        {
            return (i.flags & (DocumentMetadataItemFlags::kCompletePath | DocumentMetadataItemFlags::kNameValid)) == (DocumentMetadataItemFlags::kCompletePath | DocumentMetadataItemFlags::kNameValid);
        });
    EXPECT_TRUE(all_true);
    all_true = all_of(
        results.begin(),
        results.end(),
        [](const DocumentMetadataItem& i)
        {
            if (i.name == "C")
            {
                return i.complete_path == "A/B/C";
            }
            else if (i.name == "D")
            {
                return i.complete_path == "A/B/D";
            }
            else if (i.name == "E")
            {
                return i.complete_path == "A/B/C/E";
            }
            else if (i.name == "F")
            {
                return i.complete_path == "A/B/C/F";
            }

            return false;
        });
    EXPECT_TRUE(all_true);

    // Act

    // query for direct and indirect children of A
    results.clear();
    metadata_reader->EnumerateItems(
        id_item_a,
        true,
        DocumentMetadataItemFlags::kAllWithCompletePath,
        [&results](const auto pk, const auto item)
        {
            results.push_back(item);
            return true;
        });

    // Assert
    EXPECT_EQ(results.size(), 5);
    all_true = all_of(
        results.begin(),
        results.end(),
        [](const DocumentMetadataItem& i)
        {
            return (i.flags & (DocumentMetadataItemFlags::kCompletePath | DocumentMetadataItemFlags::kNameValid)) == (DocumentMetadataItemFlags::kCompletePath | DocumentMetadataItemFlags::kNameValid);
        });
    EXPECT_TRUE(all_true);
    all_true = all_of(
        results.begin(),
        results.end(),
        [](const DocumentMetadataItem& i)
        {
            if (i.name == "B")
            {
                return i.complete_path == "A/B";
            }
            else if (i.name == "C")
            {
                return i.complete_path == "A/B/C";
            }
            else if (i.name == "D")
            {
                return i.complete_path == "A/B/D";
            }
            else if (i.name == "E")
            {
                return i.complete_path == "A/B/C/E";
            }
            else if (i.name == "F")
            {
                return i.complete_path == "A/B/C/F";
            }

            return false;
        });
    EXPECT_TRUE(all_true);
}
