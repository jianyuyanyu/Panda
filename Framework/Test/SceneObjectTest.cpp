#include <iostream>
#include "SceneObject.hpp"

using namespace Panda;
using namespace xg;

int32_t main(int32_t argc, char** argv)
{
    int32_t result = 0;
    SceneObjectMesh         soMesh;
    SceneObjectMaterial     soMaterial;
    SceneObjectPointLight   soPointLight;
    SceneObjectSpotLight    soSpotLight;
    SceneObjectOrthogonalCamera soOrthogonalCamera;
    SceneObjectPerspectiveCamera soPerspectiveCamera;

    std::cout << soMesh << std::endl;
    std::cout << soMaterial << std::endl;
    std::cout << soPointLight << std::endl;
    std::cout << soSpotLight << std::endl;
    std::cout << soOrthogonalCamera << std::endl;
    std::cout << soPerspectiveCamera << std::endl;
    
    return result;
}