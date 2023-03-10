// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <sqlite3.h>
#include <types.h>

/// Custom Queries are found here - here we register custom extensions.
/// Currently, we extend R-Tree queries (https://sqlite.org/rtree.html).
class SqliteCustomFunctions
{
private:
    friend class CustomQueriesTest;
    static constexpr int kNumberOfArgumentsForScalarFunctionDoesIntersectWithLine = 8;
    static constexpr int kNumberOfParametersExpectedForPlane3DQuery = 6;
public:
    enum class Query
    {
        RTree_LineSegment2D,
        RTree_PlaneAabb3D,
        Scalar_DoesIntersectWithLine
    };

    static void SetupCustomQueries(sqlite3* database);
    static const char* GetQueryFunctionName(Query query);
private:
    static int LineThrough2Points2d_Query(sqlite3_rtree_query_info* info);
    static int Plane3d_Query(sqlite3_rtree_query_info* info);

    static void Free_LineThruTwoPointsD(void* pointer);
    static void Free_PlaneNormalAndDistD(void* pointer);

    static bool DoLinesIntersect(const imgdoc2::PointD& point_a1, const imgdoc2::PointD& point_a2, const imgdoc2::PointD& point_b1, const imgdoc2::PointD& point_b2);
    static bool DoAabbAndPlaneIntersect(const imgdoc2::CuboidD& aabb, const imgdoc2::Plane_NormalAndDistD& plane);
    static void ScalarFunctionDoesIntersectWithLine(sqlite3_context* context, int argc, sqlite3_value** argv);
};
