// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>
#include "../libimgdoc2/inc/imgdoc2.h"
#include "../libimgdoc2/src/db/utilities.h"

using namespace std;
using namespace imgdoc2;

static void GetColumnNameForDimension(imgdoc2::Dimension d, std::string& columnName)
{
    columnName = "Dim_" + string(1, d);
}

TEST(Db_Utilities, CreateConditionForDimQueryClauseCheckCorrectness1)
{
    CDimCoordinateQueryClause dim_query_clause;
    dim_query_clause.AddRangeClause('Z', IDimCoordinateQueryClause::RangeClause{ 1, 3 });
    const auto query_statement_and_binding_value = Utilities::CreateWhereConditionForDimQueryClause(&dim_query_clause, GetColumnNameForDimension);
    EXPECT_STREQ(get<0>(query_statement_and_binding_value).c_str(), "(([Dim_Z] > ? AND [Dim_Z] < ?))");
    ASSERT_EQ(get<1>(query_statement_and_binding_value).size(), 2);
    EXPECT_TRUE(holds_alternative<int>(get<1>(query_statement_and_binding_value)[0].value));
    EXPECT_EQ(get<int>(get<1>(query_statement_and_binding_value)[0].value), 1);
    EXPECT_TRUE(holds_alternative<int>(get<1>(query_statement_and_binding_value)[1].value));
    EXPECT_EQ(get<int>(get<1>(query_statement_and_binding_value)[1].value), 3);
}

TEST(Db_Utilities, CreateConditionForDimQueryClauseCheckCorrectness2)
{
    CDimCoordinateQueryClause dim_query_clause;
    dim_query_clause.AddRangeClause('Z', IDimCoordinateQueryClause::RangeClause{ 1, 3 });
    dim_query_clause.AddRangeClause('T', IDimCoordinateQueryClause::RangeClause{ 5, 8 });
    const auto query_statement_and_binding_value = Utilities::CreateWhereConditionForDimQueryClause(&dim_query_clause, GetColumnNameForDimension);
    EXPECT_STREQ(get<0>(query_statement_and_binding_value).c_str(), "(([Dim_T] > ? AND [Dim_T] < ?)) AND (([Dim_Z] > ? AND [Dim_Z] < ?))");
    ASSERT_EQ(get<1>(query_statement_and_binding_value).size(), 4);
    EXPECT_TRUE(holds_alternative<int>(get<1>(query_statement_and_binding_value)[0].value));
    EXPECT_EQ(get<int>(get<1>(query_statement_and_binding_value)[0].value), 5);
    EXPECT_TRUE(holds_alternative<int>(get<1>(query_statement_and_binding_value)[1].value));
    EXPECT_EQ(get<int>(get<1>(query_statement_and_binding_value)[1].value), 8);
    EXPECT_TRUE(holds_alternative<int>(get<1>(query_statement_and_binding_value)[2].value));
    EXPECT_EQ(get<int>(get<1>(query_statement_and_binding_value)[2].value), 1);
    EXPECT_TRUE(holds_alternative<int>(get<1>(query_statement_and_binding_value)[3].value));
    EXPECT_EQ(get<int>(get<1>(query_statement_and_binding_value)[3].value), 3);
}

TEST(Db_Utilities, CreateConditionForDimQueryClauseCheckCorrectness3)
{
    CDimCoordinateQueryClause dim_query_clause;
    dim_query_clause.AddRangeClause('Z', IDimCoordinateQueryClause::RangeClause{ 1, 3 });
    dim_query_clause.AddRangeClause('Z', IDimCoordinateQueryClause::RangeClause{ 10, 15 });
    const auto query_statement_and_binding_value = Utilities::CreateWhereConditionForDimQueryClause(&dim_query_clause, GetColumnNameForDimension);
    EXPECT_STREQ(get<0>(query_statement_and_binding_value).c_str(), "(([Dim_Z] > ? AND [Dim_Z] < ?) OR ([Dim_Z] > ? AND [Dim_Z] < ?))");
    ASSERT_EQ(get<1>(query_statement_and_binding_value).size(), 4);
    EXPECT_TRUE(holds_alternative<int>(get<1>(query_statement_and_binding_value)[0].value));
    EXPECT_EQ(get<int>(get<1>(query_statement_and_binding_value)[0].value), 1);
    EXPECT_TRUE(holds_alternative<int>(get<1>(query_statement_and_binding_value)[1].value));
    EXPECT_EQ(get<int>(get<1>(query_statement_and_binding_value)[1].value), 3);
    EXPECT_TRUE(holds_alternative<int>(get<1>(query_statement_and_binding_value)[2].value));
    EXPECT_EQ(get<int>(get<1>(query_statement_and_binding_value)[2].value), 10);
    EXPECT_TRUE(holds_alternative<int>(get<1>(query_statement_and_binding_value)[3].value));
    EXPECT_EQ(get<int>(get<1>(query_statement_and_binding_value)[3].value), 15);
}

TEST(Db_Utilities, CreateConditionForDimQueryClauseCheckCorrectness4)
{
    CDimCoordinateQueryClause dim_query_clause;
    dim_query_clause.AddRangeClause('Z', IDimCoordinateQueryClause::RangeClause{ 1, 1 });
    const auto query_statement_and_binding_value = Utilities::CreateWhereConditionForDimQueryClause(&dim_query_clause, GetColumnNameForDimension);
    EXPECT_STREQ(get<0>(query_statement_and_binding_value).c_str(), "(([Dim_Z] = ?))");
    ASSERT_EQ(get<1>(query_statement_and_binding_value).size(), 1);
    EXPECT_TRUE(holds_alternative<int>(get<1>(query_statement_and_binding_value)[0].value));
    EXPECT_EQ(get<int>(get<1>(query_statement_and_binding_value)[0].value), 1);
}

TEST(Db_Utilities, CreateConditionForTileInfoQueryClauseCheckCorrectness1)
{
    CTileInfoQueryClause dim_query_clause;
    dim_query_clause.AddPyramidLevelCondition(LogicalOperator::Invalid, ComparisonOperation::Equal, 5);
    const auto query_statement_and_binding_value = Utilities::CreateWhereConditionForTileInfoQueryClause(&dim_query_clause, "PyrLvl");
    EXPECT_STREQ(get<0>(query_statement_and_binding_value).c_str(), "(( [PyrLvl] = ?))");
    ASSERT_EQ(get<1>(query_statement_and_binding_value).size(), 1);
    EXPECT_TRUE(holds_alternative<int>(get<1>(query_statement_and_binding_value)[0].value));
    EXPECT_EQ(get<int>(get<1>(query_statement_and_binding_value)[0].value), 5);
}

TEST(Db_Utilities, CreateConditionForTileInfoQueryClauseCheckCorrectness2)
{
    CTileInfoQueryClause dim_query_clause;
    dim_query_clause.AddPyramidLevelCondition(LogicalOperator::Invalid, ComparisonOperation::Equal, 5);
    dim_query_clause.AddPyramidLevelCondition(LogicalOperator::And, ComparisonOperation::Equal, 2);
    dim_query_clause.AddPyramidLevelCondition(LogicalOperator::Or, ComparisonOperation::Equal, 4);
    const auto query_statement_and_binding_value = Utilities::CreateWhereConditionForTileInfoQueryClause(&dim_query_clause, "PyrLvl");
    EXPECT_STREQ(get<0>(query_statement_and_binding_value).c_str(), "(( [PyrLvl] = ?) AND ( [PyrLvl] = ?) OR ( [PyrLvl] = ?))");
    ASSERT_EQ(get<1>(query_statement_and_binding_value).size(), 3);
    EXPECT_TRUE(holds_alternative<int>(get<1>(query_statement_and_binding_value)[0].value));
    EXPECT_EQ(get<int>(get<1>(query_statement_and_binding_value)[0].value), 5);
    EXPECT_TRUE(holds_alternative<int>(get<1>(query_statement_and_binding_value)[1].value));
    EXPECT_EQ(get<int>(get<1>(query_statement_and_binding_value)[1].value), 2);
    EXPECT_TRUE(holds_alternative<int>(get<1>(query_statement_and_binding_value)[2].value));
    EXPECT_EQ(get<int>(get<1>(query_statement_and_binding_value)[2].value), 4);
}

TEST(Db_Utilities, CreateEmptyConditionForTileInfoQueryClauseCheckCorrectness)
{
    const CTileInfoQueryClause dim_query_clause;
    const auto query_statement_and_binding_value = Utilities::CreateWhereConditionForTileInfoQueryClause(&dim_query_clause, "PyrLvl");
    EXPECT_STREQ(get<0>(query_statement_and_binding_value).c_str(), "(TRUE)");
    ASSERT_EQ(get<1>(query_statement_and_binding_value).size(), 0);
}
