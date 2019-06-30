#pragma once

#include <vector>
#include <set>
#include <unordered_set>
#include "MathUtility.hpp"
// #include "Vector2D.hpp"
// #include "Vector3D.hpp"
// #include "Vector4D.hpp"
// #include "Matrix3.hpp"
// #include "Matrix4.hpp"
#include "Vector.hpp"
#include "Matrix.hpp"
#include <math.h>
#include "Utility.hpp"
#include "DCT.hpp"
#include "Bezier.hpp"
#include "Numerical.hpp"

namespace Panda
{
    typedef Vector<float, 3> Point;
    typedef std::shared_ptr<Point> PointPtr;
    typedef std::unordered_set<PointPtr> PointSet;
    typedef std::vector<PointPtr> PointList;
    typedef std::pair<PointPtr, PointPtr> Edge;
    inline bool operator==(const Edge& a, const Edge& b)
    {
        return (a.first == b.first && a.second == b.second) || (a.first == b.second && a.second == b.first);
    }
    typedef std::shared_ptr<Edge> EdgePtr;
    inline bool operator==(const EdgePtr& a, const EdgePtr& b)
    {
        return (a->first == b->first && a->second == b->second) || (a->first == b->second && a->second == b->first);
    }
    typedef std::unordered_set<EdgePtr> EdgeSet;
    typedef std::vector<EdgePtr> EdgeList;
    
}
