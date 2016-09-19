
// Original CSG.JS library by Evan Wallace (http://madebyevan.com), under the MIT license.
// GitHub: https://github.com/evanw/csg.js/
//
// C++ port by Tomasz Dabrowski (http://28byteslater.com), under the MIT license.
// GitHub: https://github.com/dabroz/csgjs-cpp/
//
// Using double instead of float
// Laurent Ribon (http://glc-lib.sourceforge.net)
// GitHub: https://github.com/laumaya/GLC_lib/
//
// Constructive Solid Geometry (CSG) is a modeling technique that uses Boolean
// operations like union and intersection to combine 3D solids. This library
// implements CSG operations on meshes elegantly and concisely using BSP trees,
// and is meant to serve as an easily understandable implementation of the
// algorithm. All edge cases involving overlapping coplanar polygons in both
// solids are correctly handled.
//
// To use this as a header file, define CSGJS_HEADER_ONLY before including this file.
//
#ifndef CSGJS_H
#define CSGJS_H

#include <algorithm>
#include <math.h>

#include <QList>

struct csgjs_vector
{
    double x, y, z;

    csgjs_vector() : x(0.0f), y(0.0f), z(0.0f) {}
    explicit csgjs_vector(double x, double y, double z) : x(x), y(y), z(z) {}
};

struct csgjs_vertex
{
    csgjs_vector pos;
    csgjs_vector normal;
    csgjs_vector uv;
};

struct csgjs_model
{
    QList<csgjs_vertex> vertices;
    QList<int> indices;
};

// public interface - not super efficient, if you use multiple CSG operations you should
// use BSP trees and convert them into model only once. Another optimization trick is
// replacing csgjs_model with your own class.

csgjs_model csgjs_union(const csgjs_model & a, const csgjs_model & b);
csgjs_model csgjs_intersection(const csgjs_model & a, const csgjs_model & b);
csgjs_model csgjs_difference(const csgjs_model & a, const csgjs_model & b);

#endif // CSGJS_H
