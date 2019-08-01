#pragma once
#include "Interface/SceneParser.hpp"

namespace Panda
{
    class DaeParser : implements SceneParser
    {
        public:
            DaeParser() = default;
            virtual ~DaeParser() = default;

            virtual std::unique_ptr<Scene> Parse(const std::string& file);
    };
}