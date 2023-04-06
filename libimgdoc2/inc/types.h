// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include <stdexcept>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace imgdoc2
{
    /// This defines the time for a primary key, used to uniquely identify a record in a table.
    typedef std::int64_t dbIndex;

    /// Defines an alias representing the "dimension". Legal dimensions are a-z and A-Z. Use IsDimensionValid(Dimension) to check for a legal dimension value.
    typedef char Dimension;

    /// Queries if the specified value is a legal dimension. Legal dimensions are a-z and A-Z.
    /// \param  dimension The dimension.
    /// \returns True if the dimension is valid, false if not.
    inline bool IsDimensionValid(Dimension dimension)
    {
        return ((dimension >= 'a' && dimension <= 'z') || (dimension >= 'A' && dimension <= 'Z'));
    }

    /// Throw an invalid_argument exception if the specified dimension is invalid.
    /// \exception std::invalid_argument Thrown when an invalid argument error condition occurs.
    /// \param  dimension The dimension.
    inline void ThrowIfDimensionInvalid(Dimension dimension)
    {
        if (!imgdoc2::IsDimensionValid(dimension))
        {
            std::ostringstream ss;
            ss << "The character '" << (isprint(dimension) ? dimension : '?') << "'=0x" << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(dimension) << " is not a valid dimension specifier.";
            throw std::invalid_argument(ss.str());
        }
    }

    /// Structure defining a point in two dimensions.
    /// \tparam t Generic type parameter.
    template <typename t>
    struct PointT
    {
        /// Default constructor which sets all properties to zero.
        PointT() :x(0), y(0) {}

        /// Constructor
        /// \param  x The x-coordinate of the point.
        /// \param  y The y-coordinate of the point.
        PointT(t x, t y) :x(x), y(y) {}

        t x;    ///< The x-coordinate of the point.
        t y;    ///< The y-coordinate of the point.
    };

    /// Structure defining a point in two dimensions with floats representing the coordinates.
    struct PointF : PointT<float>
    {
        /// Default constructor which sets all properties to zero.
        PointF() :PointT<float>() {}

        /// Constructor
        /// \param  x The x-coordinate of the point.
        /// \param  y The y-coordinate of the point.
        PointF(float x, float y) :PointT<float>(x, y) {}
    };

    /// Structure defining a point in two dimensions with doubles representing the coordinates.
    struct PointD : PointT<double>
    {
        /// Default constructor which sets all properties to zero.
        PointD() :PointT<double>() {}

        /// Constructor
        /// \param  x The x-coordinate of the point.
        /// \param  y The y-coordinate of the point.
        PointD(double x, double y) :PointT<double>(x, y) {}
    };

    /// Structure defining a point in three dimensions.
    /// \tparam t Generic type parameter.
    template <typename t>
    struct Point3dT
    {
        /// Default constructor which sets all properties to zero.
        Point3dT() :x(0), y(0), z(0) {}

        /// Constructor
        /// \param  x The x-coordinate of the point.
        /// \param  y The y-coordinate of the point.
        /// \param  z The z-coordinate of the point.
        Point3dT(t x, t y, t z) :x(x), y(y), z(z) {}

        t x;    ///< The x-coordinate of the point.
        t y;    ///< The y-coordinate of the point.
        t z;    ///< The z-coordinate of the point.
    };

    /// Structure defining a point in three dimensions with floats representing the coordinates.
    struct Point3dF : Point3dT<float>
    {
        Point3dF() :Point3dT<float>() {}

        /// Constructor.
        /// \param  x The x coordinate.
        /// \param  y The y coordinate.
        /// \param  z The z coordinate.
        Point3dF(float x, float y, float z) :Point3dT<float>(x, y, z) {}
    };

    /// Structure defining a point in three dimensions with doubles representing the coordinates.
    struct Point3dD : Point3dT<double>
    {
        Point3dD() :Point3dT<double>() {}

        /// Constructor.
        /// \param  x The x coordinate.
        /// \param  y The y coordinate.
        /// \param  z The z coordinate.
        Point3dD(double x, double y, double z) :Point3dT<double>(x, y, z) {}
    };

    /// Structure defining an axis-aligned rectangle in two dimensions.
    /// \tparam t Generic type parameter.
    template<typename t>
    struct RectangleT
    {
        /// Default constructor which sets all properties to zero.
        RectangleT() :x(0), y(0), w(0), h(0) {}

        /// Constructor
        /// \exception std::invalid_argument Thrown when an invalid argument error condition occurs.
        /// \param  x The x-coordinate of the edge point of the rectangle.
        /// \param  y The y-coordinate of the edge point of the rectangle.
        /// \param  w The width of the rectangle.
        /// \param  h The height of the rectangle.
        RectangleT(t x, t y, t w, t h)
        {
            if (w < 0 || h < 0)
            {
                throw std::invalid_argument("width and height must be non-negative");
            }

            this->x = x;
            this->y = y;
            this->w = w;
            this->h = h;
        }

        t x;    ///< The x-coordinate of the edge point of the rectangle.
        t y;    ///< The y-coordinate of the edge point of the rectangle.
        t w;    ///< The width of the rectangle.
        t h;    ///< The height of the rectangle.

        /// Query if the specified point 'p' is inside the rectangle.
        /// \param  p The point to test.
        /// \returns True if point inside, false if not.
        inline bool IsPointInside(const PointT<t>& p) const
        {
            if (this->x <= p.x && (this->x + this->w) >= p.x && this->y <= p.y && (this->y + this->h) >= p.y)
            {
                return true;
            }

            return false;
        }
    };

    /// Structure defining  an axis-aligned rectangle in two dimensions with floats representing the coordinates.
    struct RectangleF : RectangleT<float>
    {
        RectangleF() :RectangleT<float>() {}

        // Constructor.
        /// \exception std::invalid_argument Thrown when an invalid argument error condition occurs.
        /// \param  x The x-coordinate of the edge point of the rectangle.
        /// \param  y The y-coordinate of the edge point of the rectangle.
        /// \param  w The width of the rectangle.
        /// \param  h The height of the rectangle.
        RectangleF(float x, float y, float w, float h) :RectangleT<float>(x, y, w, h) {}
    };

    /// Structure defining  an axis-aligned rectangle in two dimensions with doubles representing the coordinates.
    struct RectangleD : RectangleT<double>
    {
        RectangleD() :RectangleT<double>() {}

        // Constructor.
        /// \exception std::invalid_argument Thrown when an invalid argument error condition occurs.
        /// \param  x The x-coordinate of the edge point of the rectangle.
        /// \param  y The y-coordinate of the edge point of the rectangle.
        /// \param  w The width of the rectangle.
        /// \param  h The height of the rectangle.
        RectangleD(double x, double y, double w, double h) :RectangleT<double>(x, y, w, h) {}
    };

    /// This structure defines a line in two dimensions by specifying two points on this line.
    /// Coordinates are represented by floats.
    struct LineThruTwoPointsF
    {
        PointF a;   ///< The first point on the line.
        PointF b;   ///< The second point on the line.
    };

    /// This structure defines a line in two dimensions by specifying two points on this line.
    /// Coordinates are represented by doubles.
    struct LineThruTwoPointsD
    {
        PointD a;   ///< The first point on the line.
        PointD b;   ///< The second point on the line.
    };

    template <typename t> struct Vector3dT;
    template<typename T> struct Plane_NormalAndDist;

    /// Structure defining an axis-aligned cuboid in three dimensions.
    /// \tparam t Generic type parameter.
    template<typename t>
    struct CuboidT
    {
        /// Default constructor. All properties are set to zero.
        CuboidT() :x(0), y(0), z(0), w(0), h(0), d(0) {}

        /// Constructor.
        /// \exception std::invalid_argument Thrown when an invalid argument error condition occurs.
        /// \param  x The x-coordinate of the edge point of the cuboid.
        /// \param  y The y-coordinate of the edge point of the cuboid.
        /// \param  z The z-coordinate of the edge point of the cuboid.
        /// \param  w The width of the cuboid (i.e. the extent in x-direction).
        /// \param  h The height of the cuboid (i.e. the extent in y-direction).
        /// \param  d The depth of the cuboid (i.e. the extent in z-direction).
        CuboidT(t x, t y, t z, t w, t h, t d)
        {
            if (w < 0 || h < 0 || d < 0)
            {
                throw std::invalid_argument("width and height must be non-negative");
            }

            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
            this->h = h;
            this->d = d;
        }

        t x;    ///< The x-coordinate of the edge point of the cuboid.
        t y;    ///< The y-coordinate of the edge point of the cuboid.
        t z;    ///< The z-coordinate of the edge point of the cuboid.
        t w;    ///< The width of the cuboid (i.e. the extent in x-direction).
        t h;    ///< The height of the cuboid (i.e. the extent in y-direction).
        t d;    ///< The depth of the cuboid (i.e. the extent in z-direction).

        /// Query if the specified point 'p' is inside the cubiod.
        /// \param  p The point to test.
        /// \returns True if point inside, false if not.
        bool IsPointInside(const Point3dD& p) const
        {
            if (this->x <= p.x && (this->x + this->w) >= p.x && this->y <= p.y && (this->y + this->h) >= p.y && this->z <= p.z && (this->z + this->d) >= p.z)
            {
                return true;
            }

            return false;
        }

        /// Calculate the center point of the cuboid.
        /// \returns The center point of the cuboid.
        Point3dT<t> CenterPoint() const
        {
            return Point3dT<t>(this->x + this->w / 2, this->y + this->h / 2, this->z + this->d / 2);
        }

        /// Test whether the specified cuboid is intersecting with the specified plane.
        /// \param  aabb  The axis-aligned cuboid.
        /// \param  plane The plane (given in plane-and-normal representation).
        /// \returns True if the two objects intersect; false otherwise.
        static bool DoIntersect(const imgdoc2::CuboidT<t>& aabb, const imgdoc2::Plane_NormalAndDist<t>& plane);

        /// Test whether the specified plane is intersecting with the cuboid instance.
        /// \param  plane The plane (given in plane-and-normal representation).
        /// \returns True if the two objects intersect; false otherwise.
        bool DoesIntersectWith(const imgdoc2::Plane_NormalAndDist<t>& plane) const
        {
            return DoIntersect(*this, plane);
        }
    };

    /// Structure defining an axis-aligned cuboid in three dimensions with floats representing the coordinates.
    struct CuboidF : CuboidT<float>
    {
        CuboidF() :CuboidT<float>() {}

        /// Constructor.
        /// \exception std::invalid_argument Thrown when an invalid argument error condition occurs.
        /// \param  x The x-coordinate of the edge point of the cuboid.
        /// \param  y The y-coordinate of the edge point of the cuboid.
        /// \param  z The z-coordinate of the edge point of the cuboid.
        /// \param  w The width of the cuboid (i.e. the extent in x-direction).
        /// \param  h The height of the cuboid (i.e. the extent in y-direction).
        /// \param  d The depth of the cuboid (i.e. the extent in z-direction).
        CuboidF(float x, float y, float z, float w, float h, float d) :CuboidT<float>(x, y, z, w, h, d) {}
    };

    /// Structure defining an axis-aligned cuboid in three dimensions with doubles representing the coordinates.
    struct CuboidD : CuboidT<double>
    {
        CuboidD() :CuboidT<double>() {}

        /// Constructor.
        /// \exception std::invalid_argument Thrown when an invalid argument error condition occurs.
        /// \param  x The x-coordinate of the edge point of the cuboid.
        /// \param  y The y-coordinate of the edge point of the cuboid.
        /// \param  z The z-coordinate of the edge point of the cuboid.
        /// \param  w The width of the cuboid (i.e. the extent in x-direction).
        /// \param  h The height of the cuboid (i.e. the extent in y-direction).
        /// \param  d The depth of the cuboid (i.e. the extent in z-direction).
        CuboidD(double x, double y, double z, double w, double h, double d) :CuboidT<double>(x, y, z, w, h, d) {}
    };

    /// Structure defining a vector in three dimensions.
    /// \tparam t Generic type parameter.
    template <typename t>
    struct Vector3dT
    {
        /// Default constructor. All properties are set to zero.
        Vector3dT() :x(0), y(0), z(0) {}

        /// Constructor.
        /// \param  x The component of the vector in x-direction.
        /// \param  y The component of the vector in y-direction.
        /// \param  z The component of the vector in z-direction.
        Vector3dT(t x, t y, t z) :x(x), y(y), z(z) {}

        /// Constructor.
        /// \param  p A Point3dT&lt;t&gt; to convert into a vector.
        explicit Vector3dT(const Point3dT<t> p) : Vector3dT(p.x, p.y, p.z) {}

        t x;    ///< The component of the vector in x-direction.
        t y;    ///< The component of the vector in y-direction.
        t z;    ///< The component of the vector in z-direction.

        /// Gets a normalized vector.
        /// \returns The normalized vector.
        Vector3dT<t> Normalize() const
        {
            t absVal = this->AbsoluteValue();
            return Vector3dT(this->x / absVal, this->y / absVal, this->z / absVal);
        }

        /// Calculate the squared length of the vector.
        /// \returns The squared length of the vector.
        t AbsoluteValueSquared() const
        {
            return this->x * this->x + this->y * this->y + this->z * this->z;
        }

        /// Calculate the length of the vector.
        /// \returns The length of the vector.
        t AbsoluteValue() const
        {
            return std::sqrt(this->AbsoluteValueSquared());
        }

        /// Calculate the cross-product of two vectors.
        /// \param  vectorA The first vector.
        /// \param  vectorB The second vector.
        /// \returns The cross-product of the two vectors.
        static Vector3dT<t> Cross(const Vector3dT<t>& vectorA, const Vector3dT<t>& vectorB)
        {
            return Vector3dT
            {
                vectorA.y * vectorB.z - vectorA.z * vectorB.y,
                vectorA.z * vectorB.x - vectorA.x * vectorB.z,
                vectorA.x * vectorB.y - vectorA.y * vectorB.x
            };
        }

        /// Calculate the dot-product of two vectors.
        /// \param  vectorA The first vector.
        /// \param  vectorB The second vector.
        /// \returns The dot-product of the two vectors.
        static t Dot(const Vector3dT<t>& vectorA, const Vector3dT<t>& vectorB)
        {
            return vectorA.x * vectorB.x + vectorA.y * vectorB.y + vectorA.z * vectorB.z;
        }
    };

    /// Structure defining a vector in three dimensions with floats representing the coordinates.
    struct Vector3dF : Vector3dT<float>
    {
        Vector3dF() :Vector3dT<float>() {}

        /// Constructor.
        /// \param  x The component of the vector in x-direction.
        /// \param  y The component of the vector in y-direction.
        /// \param  z The component of the vector in z-direction.
        Vector3dF(float x, float y, float z) :Vector3dT<float>(x, y, z) {}
    };

    /// Structure defining a vector in three dimensions with doubles representing the coordinates.
    struct Vector3dD : Vector3dT<double>
    {
        Vector3dD() :Vector3dT<double>() {}

        /// Constructor.
        /// \param  x The component of the vector in x-direction.
        /// \param  y The component of the vector in y-direction.
        /// \param  z The component of the vector in z-direction.
        Vector3dD(double x, double y, double z) :Vector3dT<double>(x, y, z) {}
    };

    /// Parametrization of a plane, parametrized as a normal-vector and the distance to the origin (aka "Hesse normal form").
    /// The normal must be normalized.
    /// The equation of the plane is: dot( x, normal) = distance.
    template<typename T>
    struct Plane_NormalAndDist
    {
        Vector3dT<T> normal;    ///< The normal of the plane.
        T distance;             ///< The distance of the plane to the origin.

        /// Default constructor. All properties are initialized to zero.
        Plane_NormalAndDist() :distance(0) {}

        /// Constructor.
        /// \param  n The normal.
        /// \param  d The distance to the origin.
        Plane_NormalAndDist(const Vector3dT<T>& n, T d) :normal(n), distance(d) {}

        /// Create the normal-representation of a plane for a plane defined by three points.
        /// Precondition (not checked currently) is that the points are distinct.
        /// \param  a The first point on the plane.
        /// \param  b The second point on the plane.
        /// \param  c The third point on the plane.
        /// \returns Normal-representation of the plane.
        static Plane_NormalAndDist<T> FromThreePoints(Point3dT<T> a, Point3dT<T> b, Point3dT<T> c)
        {
            const auto n = Vector3dT<T>::Cross(Vector3dT<T>(b.x - a.x, b.y - a.y, b.z - a.z), Vector3dT<T>(c.x - a.x, c.y - a.y, c.z - a.z)).Normalize();
            const auto dist = Vector3dT<T>::Dot(n, Vector3dT<T>(a));
            return Plane_NormalAndDist<T>(n, dist);
        }
    };

    /// Defines an alias representing a parametrization of a plane, parametrized as a normal-vector and the distance to the origin.
    /// Coordinates are represented by floats.
    typedef Plane_NormalAndDist<float> Plane_NormalAndDistF;

    /// Defines an alias representing a parametrization of a plane, parametrized as a normal-vector and the distance to the origin.
    /// Coordinates are represented by doubles.
    typedef Plane_NormalAndDist<double> Plane_NormalAndDistD;
    
    /*static*/template<typename t> inline bool CuboidT<t>::DoIntersect(const imgdoc2::CuboidT<t>& aabb, const imgdoc2::Plane_NormalAndDist<t>& plane)
    {
        // -> https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
        const auto centerAabb = aabb.CenterPoint();
        const Vector3dT<t> aabbExtents = Vector3dD(aabb.w / 2, aabb.h / 2, aabb.d / 2);

        // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
        const auto r = aabbExtents.x * fabs(plane.normal.x) + aabbExtents.y * fabs(plane.normal.y) + aabbExtents.z * fabs(plane.normal.z);

        // Compute distance of box center from plane
        const auto s = Vector3dT<t>::Dot(plane.normal, Vector3dT<t>(centerAabb)) - plane.distance;

        // Intersection occurs when distance s falls within [-r,+r] interval
        return fabs(s) <= r;
    }
}
