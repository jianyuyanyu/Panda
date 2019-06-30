#include <iostream>
#include <assert.h>
#include <iomanip>
#include "PandaMath.hpp"

using namespace Panda;
using namespace std;

namespace Panda
{
	Handness g_ViewHandness = Handness::kHandnessRight;
	DepthClipSpace g_DepthClipSpace = DepthClipSpace::kDepthClipNegativeOneToOne;
}

template<typename T>
ostream& Output (ostream& out, int32_t cut, int32_t count, T in[])
{
    for (int i = 0; i < count; ++i)
    {
		if (i != 0 && (i % cut == 0))
			out << endl;
        out << fixed << setprecision(6) <<  in[i] << ",";
    }

	out << endl;
    return out;
}

void VectorTest()
{
	Vector2Df x({ 55.3f, 22.1f });
    cout << "Vector2Df: " << x << endl << endl;

    Vector3Df a({1.0f, 2.0f, 3.0f});
    Vector3Df b({5.0f, 6.0f, 7.0f});

    cout << "Vector a : " << a << endl;
    cout << "Vector b : " << b << endl;

    Vector3Df c;
    c = CrossProduct(a, b);
    cout<< "Cross Product of vec a and b is: " << c << endl;
    
    float d;
    d = DotProduct(a, b);
    cout << "Dot Product of vec a and b is: " << d << endl << endl;

    Vector4Df e({-3.0f, 3.0f, 6.0f, 1.0f});
    Vector4Df f({2.0f, 0.0f, -0.7f, 0.0f});
    cout << "Vector e: " << e << endl;
    cout << "Vector f: " << f << endl;

    Vector4Df g = e + f;
    cout << "vec e + vec f: " << g << endl;
    g = e - f;
    cout << "vec e - vec f: "<< g << endl;

    g = Normalize(g);
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

	Vector3Df v({ 1.0f, 0.0f, 0.0f });

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

	Vector3Df position({ 0.0f, 0.0f, -5.0f }), lookAt({ 0.0f, 0.0f, 0.0f }), up({ 0.0f, 1.0f, 0.0f });
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
		{ 2.0f, 1.0f, 10.0f,
		1.0f, 2.0f, 3.0f,
		2.0f, 3.0f, 4.0f }
	);
	InverseMatrix(mat3, mat3);
	cout << "Inverse mat3 : " << mat3 << endl;

    Matrix4f invertable(
		{ 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		13.0f, 14.0f, 15.0f, 1.0f });
    cout << "Known Invertable Matrix: " << invertable;
    assert(InverseMatrix(invertable, invertable));
    cout << "Inverse of Matrix: " << invertable;

    Matrix4f nonInvertable({
         1.0f,  2.0f,  3.0f,  4.0f,
         5.0f,  6.0f,  7.0f,  8.0f,
         9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f
		});
    cout << "Know sigular Matrix: " << nonInvertable;
    assert(!InverseMatrix(nonInvertable, nonInvertable));
    cout << "InvertMatrix4f returns false. " << endl;
}

void TestDCTs()
{
    float pixelBlock[64] = {
        -76, -73, -67, -62, -58, -67, -64, -55,
        -65, -69, -73, -38, -19, -43, -59, -56,
        -66, -69, -60, -15,  16, -24, -62, -55,
        -65, -70, -57, - 6,  26, -22, -58, -59,
        -61, -67, -60, -24, - 2, -40, -60, -58,
        -49, -63, -68, -58, -51, -60, -70, -53,
        -43, -57, -64, -69, -73, -67, -63, -45,
        -41, -49, -59, -60, -63, -52, -50, -34
    };

    cout << "A 8x8 int pixel block: " << endl;
    Output(cout, 8, 64, pixelBlock);
    float dct[64] = {0.0};
    DCT8x8(pixelBlock, dct);
	cout << endl;
    cout << "After DCTII : " << endl;
    Output(cout, 8, 64, dct);
	cout << endl;
	cout << endl;

	float pixelBlock2[64] = {
		-416, -33, -60,  32,  48, -40,   0,   0,
		   0, -24, -56,  19,  26,   0,   0,   0,
		 -42,  13,  80, -24, -40,   0,   0,   0,
		 -42,  17,  44, -29,   0,   0,   0,   0,
		  18,   0,   0,   0,   0,   0,   0,   0,
		   0,   0,   0,   0,   0,   0,   0,   0,
		   0,   0,   0,   0,   0,   0,   0,   0,
		   0,   0,   0,   0,   0,   0,   0,   0
	};

	cout << "A 8x8 int pixel block 2: " << endl;
	Output(cout, 8, 64, pixelBlock2);
	float dct2[64] = { 0.0f };
	IDCT8x8(pixelBlock2, dct2);
	cout << endl;
	cout << "After DCTIII : " << endl;
	Output(cout, 8, 64, dct2);
	cout << endl;
}

void TestQuaternion()
{
	float root2 = sqrtf(2.0f);
	float halfRoot2 = root2 / 2.0f;
	Vector4Df p({ 1.0f, 0.0f, 0.0f, 1.0f });	// origin point <1, 0, 0>
	Vector4Df q({ 0.0f, 0.0f, halfRoot2, halfRoot2 });	 // transform along z-axis rotate 90 degrees
	Vector4Df q_({ 0.0f, 0.0f, -halfRoot2, halfRoot2 }); // Conjugate of q
	Vector4Df p1 = MulByElement(MulByElement(q, p), q_);

	cout << "p1 = " << p1 << endl;

	Vector4Df r({ halfRoot2, 0, 0, halfRoot2 }); // along x-axis rotate 90 degrees
	Vector4Df r_({ -halfRoot2, 0, 0, halfRoot2 });	 // conjugate of r
	Vector4Df p2 = MulByElement(MulByElement(r, p1), r_);
	cout << "p2 = " << p2 << endl;

	Vector4Df w = MulByElement(r, q);
	Vector4Df w_ = MulByElement(q_, r_);
	Vector4Df p3 = MulByElement(MulByElement(w, p), w_);
	cout << "w = " << w << endl;
	cout << "w_ = " << w_ << endl;
	cout << "w * p = " << MulByElement(w, p) << endl;
	cout << "p3 = " << p3 << endl;

	Vector4Df v({ 0.0f, -halfRoot2, 0.0f, halfRoot2 });
	Vector4Df v_({ 0.0f, halfRoot2, 0.0f, halfRoot2 });
	Vector4Df p4 = MulByElement(MulByElement(v, p), v_);
	cout << "p4 = " << p4 << endl;
}

int main (int argc, char** argv)
{
	cout << std::endl;

	//VectorTest();
	//MatrixTest();
	//TestDCTs();
	TestQuaternion();
	getchar();

    return 0;
}