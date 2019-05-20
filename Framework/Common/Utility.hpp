#pragma once
#include "portable.hpp"

namespace Panda {
//#define DUMP_DETAILS 1

	template<class T>
	inline void SafeRelease(T **ppInterfaceToRelease)
	{
		if (*ppInterfaceToRelease != nullptr)
		{
			(*ppInterfaceToRelease)->Release();

			(*ppInterfaceToRelease) = nullptr;
		}
	}

	ENUM(Handness)
	{
		kHandnessRight,
		kHandnessLeft,
	};

	ENUM(DepthClipSpace)
	{
		kDepthClipZeroToOne,
		kDepthClipNegativeOneToOne,
	};

	extern Handness g_EngineHandness; // DO NOT change this. Engine handness is just a showcase.
	extern Handness g_ViewHandness;
	extern DepthClipSpace g_DepthClipSpace;
}
