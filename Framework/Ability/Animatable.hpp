#pragma once
#include "Interface.hpp"

namespace Panda
{
    template <typename T>
    Ability Animatable
    {
        typedef const T ParamType;
        virtual void Update(ParamType param) = 0;
    };
}