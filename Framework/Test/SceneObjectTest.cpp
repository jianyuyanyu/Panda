#include <iostream>
#include "SceneObject.hpp"

using namespace Panda;

int32_t main(int32_t argc, char** argv)
{
    int32_t result = 0;
    SceneObjectMesh         soMesh;
    SceneObjectMaterial     soMaterial;
    SceneObjectPointLight   soPointLight;
    SceneObjectSpotLight    soSpotLight;
    SceneObjectCamera       soCamera;

    std::cout << soMesh << std::endl;
    std::cout << soMaterial << std::endl;
    std::cout << soPointLight << std::endl;
    std::cout << soSpotLight << std::endl;
    std::cout << soCamera << std::endl;
    
    return result;
}