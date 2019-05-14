#include <iostream>
#include <assert.h>
#include "PandaMath.hpp"

using namespace Panda;
using namespace std;

namespace Panda
{
	Handness g_ViewHandness = Handness::kHandnessRight;
	DepthClipSpace g_DepthClipSpace = DepthClipSpace::kDepthClipNegativeOneToOne;
}

void VectorTest()
{
    Vector2Df x(55.3f, 22.1f);
    cout << "Vector2Df: " << x << endl << endl;

    Vector3Df a = {1.0f, 2.0f, 3.0f};
    Vector3Df b = {5.0f, 6.0f, 7.0f};

    cout << "Vector a : " << a << endl;
    cout << "Vector b : " << b << endl;

    Vector3Df c;
    c = a.CrossProduct(b);
    cout<< "Cross Product of vec a and b is: " << c << endl;
    
    float d;
    d = a.DotProduct(b);
    cout << "Dot Product of vec a and b is: " << d << endl << endl;

    Vector4Df e = {-3.0f, 3.0f, 6.0f, 1.0f};
    Vector4Df f = {2.0f, 0.0f, -0.7f, 0.0f};
    cout << "Vector e: " << e << endl;
    cout << "Vector f: " << f << endl;

    Vector4Df g = e + f;
    cout << "vec e + vec f: " << g << endl;
    g = e - f;
    cout << "vec e - vec f: "<< g << endl;

    g.Normalize();
    cout << "g normalized is " << g << endl << endl;
}

void MatrixTest()
{
    Matrix4f m1;
    m1.SetIdentity();

    cout << "Identity Matrix : " << m1 << endl;

    Matrix4f mEu;
    float yaw = 0.2f, pitch = 0.3f, roll = 0.4f;
    MatrixRotationYawPitchRoll(mEu, yaw, pitch, roll);
    cout << "Matrix of yaw(" << yaw << ") pitch(" << pitch << ") roll(" << roll << "):" << mEu << endl;

    Matrix4f ry;
    float angle = PI / 2.0f;
    MatrixRotationY(ry, angle);
    cout << "Matrix of Rotation on Y(angle = " << angle << "): " << ry << endl;

    Matrix4f rz;
    MatrixRotationZ(rz, angle);
    cout << "Matrix of Rotation on Z(angle = " << angle << "): " << rz << endl;

    float x = 5.0f, y = 6.5f, z = -7.0f;
    Matrix4f translate;
    MatrixTranslation(translate, x, y, z);
    cout << "Matrix of Translation on X(" << x << ")Y" << y << ")Z(" << z << "):";
    cout << translate << endl;

    cout << "Matrix multiplication: Rotation Y * Rotation Z * Translation on X: ";
    Matrix4f transform = m1 * ry * rz * translate;
    cout << transform << endl;

    Vector3Df v = {1.0f, 0.0f, 0.0f};

    Vector3Df v1 = v;
    cout << "Vector : " << v1;
    cout << "Transfrom by Rotation Y Matrix: ";
    cout << ry;
    TransformCoord(v1, ry);
    cout << "Now the vector becomes: " << v1;
    cout << endl;

    v1 = v;
    cout << "Vector: " << v1;
    cout << "Tranform by Rotation Z Matrix: ";
    cout << rz;
    TransformCoord(v1, rz);
    cout << "Now the vector becomes: " << v1;
    cout << endl;

    v1 = v;
    cout << "Vector: " << v1;
    cout << "Transform by Translation Matrix: ";
    cout << translate;
    TransformCoord(v1, translate);
    cout << "Now the vector becomes: " << v1;
    cout << endl;

    Vector3Df position = {0.0f, 0.0f, -5.0f}, lookAt = {0.0f, 0.0f, 0.0f}, up = {0.0f, 1.0f, 0.0f};
    Matrix4f view;
    BuildViewMatrix(view, position, lookAt, up);
    cout << "View Matrix with positoin(" << position << ") lookAt(" << lookAt << ") up(" << up << "):";
    cout << view << endl;

    float fov = PI / 2.0f, aspect = 16.0f / 9.0f, near = 1.0f, far = 100.0f;
    Matrix4f perspective;
    BuildPerspectiveFovLHMatrix(perspective, fov, aspect, near, far);
    cout << "(Left-Handed) Perspective Matrix with fov(" << fov << ") aspect(" << aspect << ") near ... far(" << near << "..." << far << "):";
    cout << perspective << endl;
    BuildPerspectiveFovRHMatrix(perspective, fov, aspect, near, far);
    cout << "(Right-Handed) Perspective Matrix with fov(" << fov << ") aspect(" << aspect << ") near ... far(" << near << "..." << far << "):";
    cout << perspective << endl;

    Matrix4f mvp = perspective * view;
    cout << "MVP: " << mvp << endl;

	Matrix3f mat3(
		{ 2.0f, 1.0f, 10.0f },
		{ 1.0f, 2.0f, 3.0f },
		{ 2.0f, 3.0f, 4.0f }
	);
	mat3.SetInverse();
	cout << "Inverse mat3 : " << mat3 << endl;

    Matrix4f invertable(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        13.0f, 14.0f, 15.0f, 1.0f);
    cout << "Known Invertable Matrix: " << invertable;
    assert(invertable.SetInverse());
    cout << "Inverse of Matrix: " << invertable;

    Matrix4f nonInvertable(
         1.0f,  2.0f,  3.0f,  4.0f,
         5.0f,  6.0f,  7.0f,  8.0f,
         9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f
    );
    cout << "Know sigular Matrix: " << nonInvertable;
    assert(!nonInvertable.SetInverse());
    cout << "InvertMatrix4f returns false. " << endl;
}

int main (int argc, char** argv)
{
	cout << std::endl;

	VectorTest();
	MatrixTest();

	getchar();

    return 0;
}