#include <iostream>
#include <assert.h>

#include "PandaMath.h"

using namespace Panda;

int main()
{
	Vector3D u(1.f, 0.f, 0.f);
	Vector3D v(0.f, 1.f, 0.f);

	Vector3D n = u.CrossProduct(v);
	Vector3D n1 = v.CrossProduct(u);

	assert(n.x == 0.f && n.y == 0.f && n.z == 1.f);
	assert(n1.x == 0.f && n1.y == 0.f && n1.z == -1.f);
	
    return 0;
}