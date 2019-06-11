#pragma once

#include <vector>
#include <set>
#include <unordered_set>
#include "MathUtility.hpp"
#include "Vector2D.hpp"
#include "Vector3D.hpp"
#include "Vector4D.hpp"
#include "Matrix3.hpp"
#include "Matrix4.hpp"
#include <math.h>
#include "Utility.hpp"
#include "DCT.hpp"
#include "Bezier.hpp"
#include "Numerical.hpp"

namespace Panda
{
    void TransformCoord(Vector3Df& inVec, const Matrix4f& inMat);

    void TransformCoord(Vector4Df& inVec, const Matrix4f& inMat);

    void MatrixTranspose(Matrix4f& outMat, const Matrix4f& inMat);

    void MatrixScale(Matrix4f& outMat, const float x, const float y, const float z);

    void MatrixTranslation(Matrix4f& outMat, float x, float y, float z);

    void MatrixTranslation(Matrix4f& outMat, const Vector3Df& inVec);

    void MatrixRotationZ(Matrix4f& outMat, const float angle);

    void MatrixRotationX(Matrix4f& outMat, const float angle);

    void MatrixRotationY(Matrix4f& outMat, const float angle);

    void MatrixRotationYawPitchRoll(Matrix4f& outMat, const float yaw, const float pitch, const float roll);

    void MatrixRotationAxis(Matrix4f& outMat, const Vector3Df& inVec, const float angle);

    void MatrixRotationQuaternion(Matrix4f& outMat, const Quaternion& q);

    void BuildViewMatrix(Matrix4f& result, const Vector3Df& pos, const Vector3Df& target, const Vector3Df& up, Handness handness = Handness::kHandnessRight);

    void BuildViewMatrixLH(Matrix4f& result, const Vector3Df& pos, const Vector3Df& target, const Vector3Df& up);

    void BuildViewMatrixRH(Matrix4f& result, const Vector3Df& pos, const Vector3Df& target, const Vector3Df& up);

    void BuildPerspectiveFovMatrix(Matrix4f& result, const float fov, const float aspect, const float near, const float far, Handness handness = Handness::kHandnessRight);

    void BuildPerspectiveFovLHMatrix(Matrix4f& result, const float fov, const float aspect, const float near, const float far);

    void BuildPerspectiveFovRHMatrix(Matrix4f& result, const float fov, const float aspect, const float near, const float far);

    typedef Vector3D<float> Point;
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
