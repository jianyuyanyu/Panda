#include "PandaMath.hpp"
#include <iostream>

using namespace Panda;


int main (int argc, char** argv)
{
    Vector3Df u(1.f, 0.f, 0.f);
    Vector3Df v(0.f, 1.f, 0.f);
    Vector3Df n = u.CrossProduct(v);

    std::cout << n << std::endl;

    n = v.CrossProduct(u);

    std::cout << n << std::endl;

    return 0;
}