#pragma once

#include <unordered_map>
#include "OpenGEX/OpenGEX.h"
#include "portable.hpp"
#include "SceneNode.hpp"
#include "SceneObject.hpp"
#include "Math/Curve.hpp"
#include "Scene.hpp"
#include "Interface/SceneParser.hpp"
#include "Math/Linear.hpp"

namespace Panda
{
    class OgexParser : implements SceneParser
    {
        private:
            void ConvertOddlStructureToSceneNode(const ODDL::Structure& structure, std::shared_ptr<BaseSceneNode>& baseNode, Scene& scene);

        public:
            OgexParser() = default;
            virtual ~OgexParser() = default;

            virtual std::unique_ptr<Scene> Parse(const std::string& buf);

        private:
            bool m_UpIsYAxis;
    };
}