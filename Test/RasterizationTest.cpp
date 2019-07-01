#include <iomanip>
#include <iostream>
#include <vector>
#include "PandaMath.hpp"

using namespace Panda;
using namespace std;

namespace Panda
{
	DepthClipSpace g_DepthClipSpace = DepthClipSpace::kDepthClipZeroToOne;
}

ostream& operator<<(ostream& out, const std::vector<Pixel2D>& vec)
{
    for (int32_t i = 0; i < vec.size(); ++i)
        out << vec[i] << endl;

	return out;
}

void TestLineAlgorithm()
{
    Pixel2D pos1({0, 0}), pos2({11, 4});
    std::vector<Pixel2D> result;
	BresenhamLineAlgorithm(pos1, pos2, result);
	cout << "start point = " << pos1 << endl;
	cout << "end point = " << pos2 << endl;
	cout << "Line = " << result << endl;
	cout << endl << endl;

	Pixel2D pos3({ 0, 10 }), pos4({ 11, 7 });
	BresenhamLineAlgorithm(pos3, pos4, result);
	cout << "start point = " << pos3 << endl;
	cout << "end point = " << pos4 << endl;
	cout << "Line = " << result << endl;
	cout << endl << endl;

	Pixel2D pos5({ 1, 1 }), pos6({ 3, 6 });
	BresenhamLineAlgorithm(pos5, pos6, result);
	cout << "start point = " << pos5 << endl;
	cout << "end point = " << pos6 << endl;
	cout << "Line = " << result << endl;
	cout << endl << endl;

	Pixel2D pos7({ 1, 7 }), pos8({ 3, 1 });
	BresenhamLineAlgorithm(pos7, pos8, result);
	cout << "start point = " << pos7 << endl;
	cout << "end point = " << pos8 << endl;
	cout << "Line = " << result << endl;
	cout << endl << endl;

}

int main()
{
    TestLineAlgorithm();

    getchar();
    return 0;
}