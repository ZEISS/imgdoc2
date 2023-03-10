// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include  <limits>
#include "custom_functions.h"
#include <stdexcept> 

#include "exceptions.h"

using namespace imgdoc2;

/*static*/const char* SqliteCustomFunctions::GetQueryFunctionName(Query query)
{
    switch (query)
    {
    case Query::RTree_LineSegment2D:
        return  "LineThroughPoints2d";
    case Query::RTree_PlaneAabb3D:
        return "PlaneNormalDistance3d";
    case Query::Scalar_DoesIntersectWithLine:
        return "IntersectsWithLine";
    }

    throw std::invalid_argument("Unknown enumeration");
}

/*static*/void SqliteCustomFunctions::SetupCustomQueries(sqlite3* database)
{
    // TODO(JBL): 
    // * Maybe consider this https://www.sqlite.org/c3ref/auto_extension.html instead of registering this stuff here by hand.
    // * It would also be nice to have a loadable extension with this functionality (https://www.sqlite.org/loadext.html).
    auto return_code = sqlite3_rtree_query_callback(
        database,
        SqliteCustomFunctions::GetQueryFunctionName(SqliteCustomFunctions::Query::RTree_LineSegment2D),
        SqliteCustomFunctions::LineThrough2Points2d_Query,
        nullptr,
        nullptr);
    if (return_code != SQLITE_OK)
    {
        throw database_exception("Error registering \"RTree_LineSegment2D\".", return_code);
    }

    return_code = sqlite3_rtree_query_callback(
        database,
        SqliteCustomFunctions::GetQueryFunctionName(SqliteCustomFunctions::Query::RTree_PlaneAabb3D),
        SqliteCustomFunctions::Plane3d_Query,
        nullptr,
        nullptr);
    if (return_code != SQLITE_OK)
    {
        throw database_exception("Error registering \"RTree_PlaneAabb3D\".", return_code);
    }

    return_code = sqlite3_create_function_v2(
        database,
        SqliteCustomFunctions::GetQueryFunctionName(SqliteCustomFunctions::Query::Scalar_DoesIntersectWithLine),
        kNumberOfArgumentsForScalarFunctionDoesIntersectWithLine,
        SQLITE_UTF8 | SQLITE_DETERMINISTIC | SQLITE_DIRECTONLY,
        nullptr,
        SqliteCustomFunctions::ScalarFunctionDoesIntersectWithLine,
        nullptr,
        nullptr,
        nullptr);
    if (return_code != SQLITE_OK)
    {
        throw database_exception("Error registering \"Scalar_DoesIntersectWithLine\".", return_code);
    }
}

/*static*/int SqliteCustomFunctions::LineThrough2Points2d_Query(sqlite3_rtree_query_info* info)
{
    auto* pLine = static_cast<LineThruTwoPointsD*>(info->pUser);
    if (pLine == nullptr)
    {
        /* If pUser is still 0, then the parameter values have not been tested
        ** for correctness or stored into a "LineThruTwoPoints" structure yet. Do this now. */

        /* This geometry callback is for use with a 2-dimensional r-tree table.
        ** Return an error if the table does not have exactly 2 dimensions. */
        if (info->nCoord != 4)
        {
            return SQLITE_ERROR;
        }

        /* Test that the correct number of parameters (4) have been supplied,
        */
        if (info->nParam != 4)
        {
            return SQLITE_ERROR;
        }

        /*Allocate a structure to cache parameter data in.Return SQLITE_NOMEM
        ** if the allocation fails.*/
        pLine = static_cast<LineThruTwoPointsD*>(info->pUser = sqlite3_malloc(sizeof(LineThruTwoPointsD)));
        if (pLine == nullptr)
        {
            return SQLITE_NOMEM;
        }

        info->xDelUser = Free_LineThruTwoPointsD;

        // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic) pointer-arithmetic is fine here
        pLine->a.x = info->aParam[0];
        pLine->a.y = info->aParam[1];
        pLine->b.x = info->aParam[2];
        pLine->b.y = info->aParam[3];
        // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic) pointer-arithmetic is fine here
    const RectangleD rect(info->aCoord[0], info->aCoord[2], info->aCoord[1] - info->aCoord[0], info->aCoord[3] - info->aCoord[2]);

    // check whether the start-/end-point is inside the rectangle
    const bool firstPointInside = rect.IsPointInside(pLine->a);
    const bool secondPointInside = rect.IsPointInside(pLine->b);
    if (firstPointInside && secondPointInside)
    {
        // if both are inside, we report "fully within"
        info->eWithin = FULLY_WITHIN;
    }
    else
    {
        if (firstPointInside || secondPointInside)
        {
            // if one of the start-/end-point is inside - then report "partly within"
            info->eWithin = PARTLY_WITHIN;
        }

        // now we determine whether the line-segment "pLine" intersects with the diagonals
        if (SqliteCustomFunctions::DoLinesIntersect(pLine->a, pLine->b, PointD(rect.x, rect.y), PointD(rect.x + rect.w, rect.y + rect.h)) ||
            SqliteCustomFunctions::DoLinesIntersect(pLine->a, pLine->b, PointD(rect.x, rect.y + rect.h), PointD(rect.x + rect.w, rect.y)))
        {
            info->eWithin = PARTLY_WITHIN;
        }

        else
        {
            info->eWithin = NOT_WITHIN;
        }
    }

    info->rScore = info->iLevel;
    return SQLITE_OK;
}

/*static*/int SqliteCustomFunctions::Plane3d_Query(sqlite3_rtree_query_info* info)
{
    auto* pPlane = static_cast<Plane_NormalAndDistD*>(info->pUser);
    if (pPlane == nullptr)
    {
        /* If pUser is still 0, then the parameter values have not been tested
        ** for correctness or stored into a "LineThruTwoPoints" structure yet. Do this now. */

        /* This geometry callback is for use with a 3-dimensional r-tree table.
        ** Return an error if the table does not have exactly 3 dimensions. */
        if (info->nCoord != kNumberOfParametersExpectedForPlane3DQuery)
        {
            return SQLITE_ERROR;
        }

        /* Test that the correct number of parameters (4) have been supplied */
        if (info->nParam != 4)
        {
            return SQLITE_ERROR;
        }

        /*Allocate a structure to cache parameter data in.Return SQLITE_NOMEM
        ** if the allocation fails.*/
        pPlane = static_cast<Plane_NormalAndDistD*>(info->pUser = sqlite3_malloc(sizeof(Plane_NormalAndDistD)));
        if (pPlane == nullptr)
        {
            return SQLITE_NOMEM;
        }

        info->xDelUser = Free_PlaneNormalAndDistD;
        // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic) pointer-arithmetic is fine here
        pPlane->normal.x = info->aParam[0];
        pPlane->normal.y = info->aParam[1];
        pPlane->normal.z = info->aParam[2];
        pPlane->distance = info->aParam[3];
        // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers) pointer-arithmetic is fine here
    CuboidD aabb(info->aCoord[0], info->aCoord[2], info->aCoord[4], info->aCoord[1] - info->aCoord[0], info->aCoord[3] - info->aCoord[2], info->aCoord[5] - info->aCoord[4]);
    const bool doIntersect = SqliteCustomFunctions::DoAabbAndPlaneIntersect(aabb, *pPlane);
    if (doIntersect)
    {
        info->eWithin = PARTLY_WITHIN;
    }
    else
    {
        info->eWithin = NOT_WITHIN;
    }

    info->rScore = info->iLevel;
    return SQLITE_OK;
}

/*static*/void SqliteCustomFunctions::Free_LineThruTwoPointsD(void* pointer)
{
    sqlite3_free(pointer);
}

/*static*/void SqliteCustomFunctions::Free_PlaneNormalAndDistD(void* pointer)
{
    sqlite3_free(pointer);
}

/*static*/bool SqliteCustomFunctions::DoLinesIntersect(const imgdoc2::PointD& point_a1, const imgdoc2::PointD& point_a2, const imgdoc2::PointD& point_b1, const imgdoc2::PointD& point_b2)
{
    const PointD point_b(point_a2.x - point_a1.x, point_a2.y - point_a1.y);
    const PointD point_d(point_b2.x - point_b1.x, point_b2.y - point_b1.y);

    const double bDotDPerp = point_b.x * point_d.y - point_b.y * point_d.x;

    // if b dot d == 0, it means the lines are parallel so have infinite intersection points
    if (std::fabs(bDotDPerp) <= std::numeric_limits<double>::epsilon())
    {
        return false;
    }

    const PointD point_c(point_b1.x - point_a1.x, point_b1.y - point_a1.y);// = point_b1 - point_a1;
    const double slope_t = (point_c.x * point_d.y - point_c.y * point_d.x) / bDotDPerp;
    if (slope_t < 0 || slope_t > 1)
    {
        return false;
    }

    const double slope_u = (point_c.x * point_b.y - point_c.y * point_b.x) / bDotDPerp;
    if (slope_u < 0 || slope_u > 1)
    {
        // NOLINTNEXTLINE(readability-simplify-boolean-expr)
        return false;
    }

    return true;
}

/*static*/bool SqliteCustomFunctions::DoAabbAndPlaneIntersect(const imgdoc2::CuboidD& aabb, const imgdoc2::Plane_NormalAndDistD& plane)
{
    return aabb.DoesIntersectWith(plane);
}

/*static*/void SqliteCustomFunctions::ScalarFunctionDoesIntersectWithLine(sqlite3_context* context, int argc, sqlite3_value** argv)
{
    if (argc != kNumberOfArgumentsForScalarFunctionDoesIntersectWithLine)
    {
        return  sqlite3_result_null(context);
    }

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic) pointer-arithmetic is fine here
    const double rect_x = sqlite3_value_double(argv[0]);
    const double rect_y = sqlite3_value_double(argv[1]);
    const double rect_width = sqlite3_value_double(argv[2]);
    const double rect_height = sqlite3_value_double(argv[3]);
    const double p1x = sqlite3_value_double(argv[4]);
    const double p1y = sqlite3_value_double(argv[5]);
    const double p2x = sqlite3_value_double(argv[6]);
    const double p2y = sqlite3_value_double(argv[7]);
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    const RectangleD rect(rect_x, rect_y, rect_width, rect_height);
    const LineThruTwoPointsD twoPoints{ {p1x, p1y}, {p2x, p2y} };

    bool doesIntersect = false;

    // check whether the start-/end-point is inside the rectangle
    if (rect.IsPointInside(twoPoints.a) || rect.IsPointInside(twoPoints.b))
    {
        // if at least one point is inside the rectangle, then we are done
        doesIntersect = true;
    }
    else
    {
        // now we determine whether the line-segment "twoPoints" intersects with the diagonals
        if (SqliteCustomFunctions::DoLinesIntersect(twoPoints.a, twoPoints.b, PointD(rect.x, rect.y), PointD(rect.x + rect.w, rect.y + rect.h)) ||
            SqliteCustomFunctions::DoLinesIntersect(twoPoints.a, twoPoints.b, PointD(rect.x, rect.y + rect.h), PointD(rect.x + rect.w, rect.y)))
        {
            doesIntersect = true;
        }
    }

    return sqlite3_result_int(context, doesIntersect ? 1 : 0);
}
