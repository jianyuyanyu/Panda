#include <windowsx.h>
#include <tchar.h>

#include <d2d1.h>

ID2D1Factory				*pFactory = nullptr;
ID2D1HwndRenderTarget		*pRenderTarget = nullptr;
ID2D1SolidColorBrush		*pLightSlateGrayBrush = nullptr;
ID2D1SolicColorBrush		*pCornflowerBlueBrush = nullptr;

template<class T>
inline void SafeRelease(T** ppInterfaceToRelease) {
	if (*ppInterfaceToRelease != nullptr) {
		(*ppInterfaceToRelease) ->Release();
		
		(*ppInterfaceToRelease) = nullptr;
	}
}