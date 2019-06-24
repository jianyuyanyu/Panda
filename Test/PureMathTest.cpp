#include "Vector.hpp"
#include "Matrix.hpp"
#include "MatrixOps.hpp"
#include <iostream>
#include <string>

using namespace Panda;
using namespace std;

int main()
{
#ifdef NEED
    Vector<float, 4> v1;
    v1.Set(1.0f);
    cout << "v1 = " << v1 << endl;
	cout << "v1 * 0.5f = " << v1 * 0.5f << endl;
	cout << "v1 / 0.5f = " << v1 / 0.5f << endl;

	Vector<float, 4> v2;
	v2.Set(2.0f);
	cout << "v2 = " << v2 << endl;
	cout << "v1 + v2 = " << v1 + v2 << endl;
	cout << "v1 dot product v2 = " << DotProduct(v1, v2) << endl;

	Matrix<float, 4, 3> matrix4x3;
	matrix4x3.Set(2.0f);
	Matrix<float, 3, 2> matrix3x2;
	matrix3x2.Set(3.0f);
	cout << "matrix4x3 = " << matrix4x3 << endl;
	cout << "matrix3x2 = " << matrix3x2 << endl;
	cout << "matrix4x3 * matrix3x2 = " << matrix4x3 * matrix3x2 << endl;

	float list[] = {
		1.0f, 1.0f, 1.0f,
		2.0f, 2.0f, 2.0f,
		3.0f, 3.0f, 4.0f
	};
	Matrix<float, 3, 3> mat(list);
	cout << "mat = " << mat << endl;

	float list[] = {
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 3.0f,
		0.0f, 3.0f, 4.0f
	};
	Matrix<float, 4, 3> mat(list);
	cout << "mat = " << mat << endl;
	int row = 0;
	ChangeRows(mat, row);
	cout << "mat after change rows(" << row << ") = " << mat << endl;

	float list[] = {
		1.0f, 2.0f, 1.0f, 3.0f,
		3.0f, -1.0f, -3.0f, -1.0f,
		2.0f, 3.0f, -3.0f, -1.0f,
	};
	float list1[] = {
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 3.0f,
		-1.0f, 2.0f, -2.0f
	};
	float list2[] = {
	1.0f, 2.0f, 1.0f, 1.0f,
	2.0f, -1.0f, 1.0f, 2.0f,
	4.0f, 3.0f, 3.0f, 4.0f,
	3.0f, 1.0f, 2.0f, 3.0f
	};
	float list3[] = {
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f,
		1.0f, 1.0f, 1.0f, 2.0f, 2.0f, 3.0f,
		1.0f, 1.0f, 1.0f, 2.0f, 3.0f, 2.0f
	};
	float list4[] = {
		1, 4, 3, 1, 0, 0,
		-1, -2, 0, 0, 1, 0,
		2, 2, 3, 0, 0, 1
	};
	float list5[] = {
		-1, 1, 1, 0,
		1, 0, 0, 1
	};
	float list6[] = {
		3, 0, 1, 0,
		9, 3, 0, 1
	};
	float list7[] = {
		1, 1, 1, 1, 0, 0,
		0, 1, 1, 0, 1, 0,
		0, 0, 1, 0, 0, 1
	};
	float list8[] = {
	2, 0, 5, 1, 0, 0,
	0, 3, 0, 0, 1, 0,
	1, 0, 3, 0, 0, 1
	};
	float list9[] = {
	-1, -3, -3, 1, 0, 0,
	2, 6, 1, 0, 1, 0,
	3, 8, 3, 0, 0, 1
	};
	float list10[] = {
		1, 0, 1, 1, 0, 0,
		-1, 1, 1, 0, 1, 0,
		-1, -2, -3, 0, 0, 1
	};
	float list11[] = {
		3, 6, 1, 0,
		2, 4, 0, 1
	};
	Matrix<float, 2, 4> mat(list11);
	cout << "mat = " << mat << endl;
	cout << "After GaussJordanReduction ---" << endl;
	//Matrix<float, 4, 4> result = GaussianElimination(mat);
	Matrix<float, 2, 4> result = GaussJordanReduction(mat);
	cout << result << endl;

	float list1[] =
	{
		2, 4, 2,
		1, 5, 2,
		4, -1, 9
	};
	float list2[] =
	{
		3, 1,
		9, 5
	};

	float list3[] =
	{
		2, 4,
		-2, 1
	};
	float list4[] =
	{
		1, 1, 1,
		3, 5, 6,
		-2, 2, 7
	};
	float list5[] =
	{
		2, 1, 2,
		4, 1, -2,
		-6, -3, 4
	};
	float list6[] =
	{
		1, 1, 1,
		2, 4, 1,
		-3, 1, -2
	};
	Matrix<float, 3, 3> mat(list6);
	cout << "origin mat = " << mat << endl;
	Matrix<float, 3, 3> L;
	Matrix<float, 3, 3> U;
	bool success = LUFactorization(mat, L, U);
	cout << "After LU factorization---" << endl;
	cout << "L = " << L << endl;
	cout << "U = " << U << endl;
	cout << "L * U = " << L * U << endl;

    float list1[] = 
    {
		3, 1,
		9, 5
    };
	float list2[] =
	{
		2, 4, 2,
		1, 5, 2,
		4, -1, 9
	};
	float list3[] = 
	{
		0, 3, 1, 1,
		1, 2, -2, 7,
		2, 5, 4, -1
	};
    Matrix<float, 3, 4> mat(list3);
    cout << "origin mat = " << mat << endl;
    Matrix<float, 3, 4> result = GaussianEliminationImprove(mat);
    cout << "After gaussion improve = " << endl;
	cout << result << endl;
	
	Matrix<float, 3, 4> result1 = GaussianElimination(mat);
	cout << endl << "After gaussion = " << endl;
	cout << result1 << endl;


	float list1[] =
	{
		1, 0,
		0, 1
	};
	float list2[] =
	{
		1, 4,
		-2, 2
	};
	float list3[] = {
		0.5, 0.5,
		0.5, 0.5
	};
	float list4[] = {
		0, 5, 1,
		2, 3, 1,
		1, 2, 2
	};
	float list5[] = {
		3, -1, -2,
		-1, 2, -7,
		4, 1, 4
	};
	Matrix<float, 3, 3> mat(list5);
	std::cout << "mat = " << mat << endl;
	std::cout << "FrobeniusNorm of mat = " << FrobeniusNorm(mat) << endl;
	std::cout << "MatrixNormByVectorNorm(1) of mat = " << MatrixNormByVectorNorm(mat, 1) << endl;
	const uint32_t INFI = 0xFFFFFFFF;
	std::cout << "MatrixNormByVectorNorm(INFINITY) of mat = " << MatrixNormByVectorNorm(mat, INFI) << endl;

	float list1[] = { 
		-1, 1.5, 0.5,
		2, 8, 8,
		-2, -7, 1
	};
	float alpha = 7;
	Matrix<float, 3, 3> mat(list1);
	Matrix<float, 3, 3> Q, R;
	QRMethodWithHouseholder(mat, Q, R);

	float list1[] = {
		5, 2, 2,
		-2, 1, -2,
		-3, -4, 2
	};
	Matrix<float, 3, 3> mat(list1);
	int32_t iterCount = 100;
	Vector<float, 3> vec = SolveMaxEigenvectorWithPowerMethod(mat, iterCount);
	cout << "The " << iterCount << " iter count value is = " << vec << endl;


	float list1[] = {
		2, 5,
		1, 10
	};
	Matrix<float, 2, 2> mat(list1);
	Matrix<float, 2, 2> Q;
	Matrix<float, 2, 2> R;
	GramSchmidt(mat, Q, R);
	cout << "origin mat = " << mat << endl;
	cout << "After QR decomposition --- " << endl;
	cout << "Q = " << Q << endl;
	cout << "R = " << R << endl;
	cout << "Matrix Q * R = " << Q * R << endl;

	float sqrt2_1 = 1.0f / std::sqrt(2);
	float listQ1[] = {
		sqrt2_1, sqrt2_1,
		sqrt2_1, -sqrt2_1,
		0, 0
	};
	float listR1[] = {
		1, 1,
		0, 1
	};

	float listB1[] = { 1, 1, 1 };
	Vector<float, 3> b(listB1);
	Vector<float, 2> x;
	Matrix<float, 3, 2> Q(listQ1);
	Matrix<float, 2, 2> R(listR1);
	LeasetSequaresMethod(Q, R, b, x);
	cout << "Q = " << Q << endl;
	cout << "R = " << R << endl;
	cout << "b = " << b << endl;
	cout << "result x = " << x << endl;
#endif
	float sqrt2_1 = 1.0f / std::sqrt(2);
	float sqrt2 = std::sqrt(2);
	float listQ2[] = {
		1, 0, 0,
		0, sqrt2_1, -sqrt2_1,
		0, sqrt2_1, sqrt2_1,
	};
	float listR2[] = {
		1, 1, 0,
		0, 1, 0,
		0, 0, 0
	};
	float listB2[] = { 1, sqrt2, -sqrt2 };
	Vector<float, 3> b(listB2);
	Vector<float, 3> x;
	Matrix<float, 3, 3> Q(listQ2);
	Matrix<float, 3, 3> R(listR2);
	LeastSequaresMethod(Q, R, b, x);
	cout << "Q = " << Q << endl;
	cout << "R = " << R << endl;
	cout << "b = " << b << endl;
	cout << "result x = " << x << endl;

    getchar();
    return 0;
}