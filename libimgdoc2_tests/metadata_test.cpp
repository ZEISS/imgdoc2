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
    vector<dbIndex> primary_keys;
    vector<DocumentMetadataItem> items;
    const auto metadata_reader = doc->GetDocumentMetadataReader();
    metadata_reader->EnumerateItems(
        id_item_b,
        false,
        DocumentMetadataItemFlags::All,
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
    EXPECT_TRUE((items[index].flags & DocumentMetadataItemFlags::All) == DocumentMetadataItemFlags::All);
    EXPECT_STREQ(items[index].name.c_str(), "C");
    ASSERT_EQ(items[index].type, DocumentMetadataType::Text);
    EXPECT_STREQ(get<string>(items[index].value).c_str(), "Testtext");
    EXPECT_EQ(items[index].primary_key, id1);

    result_item_iterator = find_if(primary_keys.begin(), primary_keys.end(), [=](const auto& pk) { return pk == id2; });
    ASSERT_NE(result_item_iterator, primary_keys.end());
    index = distance(primary_keys.begin(), result_item_iterator);
    EXPECT_TRUE((items[index].flags & DocumentMetadataItemFlags::All) == DocumentMetadataItemFlags::All);
    EXPECT_STREQ(items[index].name.c_str(), "D");
    ASSERT_EQ(items[index].type, DocumentMetadataType::Text);
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

    const auto id_item_b = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B", DocumentMetadataType::Null, std::monostate());
    const auto id1 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C", DocumentMetadataType::Text, IDocumentMetadataWrite::metadata_item_variant("Testtext"));
    const auto id2 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/D", DocumentMetadataType::Text, IDocumentMetadataWrite::metadata_item_variant("Testtext2"));
    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C/E", DocumentMetadataType::Text, IDocumentMetadataWrite::metadata_item_variant("Testtext3"));
    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C/F", DocumentMetadataType::Text, IDocumentMetadataWrite::metadata_item_variant("Testtext4"));

    // Act
    vector<dbIndex> primary_keys;
    vector<DocumentMetadataItem> items;
    const auto metadata_reader = doc->GetDocumentMetadataReader();
    metadata_reader->EnumerateItems(
        id_item_b,
        false,                                      // Note: we instruct "recursive=false", so we expect only the the two direct leaf nodes of 'B' - 'C' and 'D'
        DocumentMetadataItemFlags::All,
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
    EXPECT_TRUE((items[index].flags & DocumentMetadataItemFlags::All) == DocumentMetadataItemFlags::All);
    EXPECT_STREQ(items[index].name.c_str(), "C");
    ASSERT_EQ(items[index].type, DocumentMetadataType::Text);
    EXPECT_STREQ(get<string>(items[index].value).c_str(), "Testtext");
    EXPECT_EQ(items[index].primary_key, id1);

    result_item_iterator = find_if(primary_keys.begin(), primary_keys.end(), [=](const auto& pk) { return pk == id2; });
    ASSERT_NE(result_item_iterator, primary_keys.end());
    index = distance(primary_keys.begin(), result_item_iterator);
    EXPECT_TRUE((items[index].flags & DocumentMetadataItemFlags::All) == DocumentMetadataItemFlags::All);
    EXPECT_STREQ(items[index].name.c_str(), "D");
    ASSERT_EQ(items[index].type, DocumentMetadataType::Text);
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

    const auto id_item_b = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B", DocumentMetadataType::Null, std::monostate());
    const auto id1 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C", DocumentMetadataType::Text, IDocumentMetadataWrite::metadata_item_variant("Testtext"));
    const auto id2 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/D", DocumentMetadataType::Text, IDocumentMetadataWrite::metadata_item_variant("Testtext2"));
    const auto id3 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C/E", DocumentMetadataType::Text, IDocumentMetadataWrite::metadata_item_variant("Testtext3"));
    const auto id4 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C/F", DocumentMetadataType::Text, IDocumentMetadataWrite::metadata_item_variant("Testtext4"));

    // Act
    vector<dbIndex> primary_keys;
    vector<DocumentMetadataItem> items;
    const auto metadata_reader = doc->GetDocumentMetadataReader();
    metadata_reader->EnumerateItems(
        id_item_b,
        true,                                      // Note: we instruct "recursive=true", so we expect to have all 4 leaf nodes of 'B' : 'C', 'D', 'E' and 'F'
        DocumentMetadataItemFlags::All,
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
    EXPECT_TRUE((items[index].flags & DocumentMetadataItemFlags::All) == DocumentMetadataItemFlags::All);
    EXPECT_STREQ(items[index].name.c_str(), "C");
    ASSERT_EQ(items[index].type, DocumentMetadataType::Text);
    EXPECT_STREQ(get<string>(items[index].value).c_str(), "Testtext");
    EXPECT_EQ(items[index].primary_key, id1);

    result_item_iterator = find_if(primary_keys.begin(), primary_keys.end(), [=](const auto& pk) { return pk == id2; });
    ASSERT_NE(result_item_iterator, primary_keys.end());
    index = distance(primary_keys.begin(), result_item_iterator);
    EXPECT_TRUE((items[index].flags & DocumentMetadataItemFlags::All) == DocumentMetadataItemFlags::All);
    EXPECT_STREQ(items[index].name.c_str(), "D");
    ASSERT_EQ(items[index].type, DocumentMetadataType::Text);
    EXPECT_STREQ(get<string>(items[index].value).c_str(), "Testtext2");
    EXPECT_EQ(items[index].primary_key, id2);

    result_item_iterator = find_if(primary_keys.begin(), primary_keys.end(), [=](const auto& pk) { return pk == id3; });
    ASSERT_NE(result_item_iterator, primary_keys.end());
    index = distance(primary_keys.begin(), result_item_iterator);
    EXPECT_TRUE((items[index].flags & DocumentMetadataItemFlags::All) == DocumentMetadataItemFlags::All);
    EXPECT_STREQ(items[index].name.c_str(), "E");
    ASSERT_EQ(items[index].type, DocumentMetadataType::Text);
    EXPECT_STREQ(get<string>(items[index].value).c_str(), "Testtext3");
    EXPECT_EQ(items[index].primary_key, id3);

    result_item_iterator = find_if(primary_keys.begin(), primary_keys.end(), [=](const auto& pk) { return pk == id4; });
    ASSERT_NE(result_item_iterator, primary_keys.end());
    index = distance(primary_keys.begin(), result_item_iterator);
    EXPECT_TRUE((items[index].flags & DocumentMetadataItemFlags::All) == DocumentMetadataItemFlags::All);
    EXPECT_STREQ(items[index].name.c_str(), "F");
    ASSERT_EQ(items[index].type, DocumentMetadataType::Text);
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

    const auto id1 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C", DocumentMetadataType::Text, IDocumentMetadataWrite::metadata_item_variant("Testtext"));
    const auto id2 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/D", DocumentMetadataType::Text, IDocumentMetadataWrite::metadata_item_variant("Testtext2"));

    // Act
    vector<dbIndex> items;
    const auto metadata_reader = doc->GetDocumentMetadataReader();
    metadata_reader->EnumerateItemsForPath(
        "",
        true,
        DocumentMetadataItemFlags::All,
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

    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B", DocumentMetadataType::Null, std::monostate());
    const auto id1 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C", DocumentMetadataType::Text, IDocumentMetadataWrite::metadata_item_variant("Testtext"));
    const auto id2 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/D", DocumentMetadataType::Text, IDocumentMetadataWrite::metadata_item_variant("Testtext2"));

    // Act
    vector<dbIndex> primary_keys;
    vector<DocumentMetadataItem> items;
    const auto metadata_reader = doc->GetDocumentMetadataReader();
    metadata_reader->EnumerateItemsForPath(
        "A/B",
        false,
        DocumentMetadataItemFlags::All,
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
    EXPECT_TRUE((items[index].flags & DocumentMetadataItemFlags::All) == DocumentMetadataItemFlags::All);
    EXPECT_STREQ(items[index].name.c_str(), "C");
    ASSERT_EQ(items[index].type, DocumentMetadataType::Text);
    EXPECT_STREQ(get<string>(items[index].value).c_str(), "Testtext");
    EXPECT_EQ(items[index].primary_key, id1);

    result_item_iterator = find_if(primary_keys.begin(), primary_keys.end(), [=](const auto& pk) { return pk == id2; });
    ASSERT_NE(result_item_iterator, primary_keys.end());
    index = distance(primary_keys.begin(), result_item_iterator);
    EXPECT_TRUE((items[index].flags & DocumentMetadataItemFlags::All) == DocumentMetadataItemFlags::All);
    EXPECT_STREQ(items[index].name.c_str(), "D");
    ASSERT_EQ(items[index].type, DocumentMetadataType::Text);
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

    const auto id_item_b = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B", DocumentMetadataType::Null, std::monostate());
    const auto id1 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C", DocumentMetadataType::Text, IDocumentMetadataWrite::metadata_item_variant("Testtext"));
    const auto id2 = metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/D", DocumentMetadataType::Text, IDocumentMetadataWrite::metadata_item_variant("Testtext2"));
    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C/E", DocumentMetadataType::Text, IDocumentMetadataWrite::metadata_item_variant("Testtext3"));
    metadata_writer->UpdateOrCreateItemForPath(true, true, "A/B/C/F", DocumentMetadataType::Text, IDocumentMetadataWrite::metadata_item_variant("Testtext4"));

    // Act
    vector<dbIndex> primary_keys;
    vector<DocumentMetadataItem> items;
    const auto metadata_reader = doc->GetDocumentMetadataReader();
    metadata_reader->EnumerateItemsForPath(
        "A/B",
        false,                                      // Note: we instruct "recursive=false", so we expect only the the two direct leaf nodes of 'B' - 'C' and 'D'
        DocumentMetadataItemFlags::All,
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
    EXPECT_TRUE((items[index].flags & DocumentMetadataItemFlags::All) == DocumentMetadataItemFlags::All);
    EXPECT_STREQ(items[index].name.c_str(), "C");
    ASSERT_EQ(items[index].type, DocumentMetadataType::Text);
    EXPECT_STREQ(get<string>(items[index].value).c_str(), "Testtext");
    EXPECT_EQ(items[index].primary_key, id1);

    result_item_iterator = find_if(primary_keys.begin(), primary_keys.end(), [=](const auto& pk) { return pk == id2; });
    ASSERT_NE(result_item_iterator, primary_keys.end());
    index = distance(primary_keys.begin(), result_item_iterator);
    EXPECT_TRUE((items[index].flags & DocumentMetadataItemFlags::All) == DocumentMetadataItemFlags::All);
    EXPECT_STREQ(items[index].name.c_str(), "D");
    ASSERT_EQ(items[index].type, DocumentMetadataType::Text);
    EXPECT_STREQ(get<string>(items[index].value).c_str(), "Testtext2");
    EXPECT_EQ(items[index].primary_key, id2);
}
