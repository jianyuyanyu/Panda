#pragma once

#include <string>
#include "Interface.hpp"
#include "Scene.hpp"

namespace Panda
{
    Interface SceneParser
    {
        public:
            virtual std::unique_ptr<Scene> Parse(const std::string& buf) = 0;
    };
}