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

HRESULT CreateGraphicsResources(HWND hWnd) {
	HRESULT hr = S_OK;
	if (pRenderTarget == nullptr) {
		RECT rc;
		GetClientRect(hWnd, &rc);
		
		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left,
										rc.bottom - rc.top);
										
		hr = pFactory ->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(hWnd, size),
			&pRenderTarget);
			
		if (SUCCEEDED(hr)) {
			hr = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightSlateGray), &pLightSlateGrayBrush);
		}
		
		if (SUCCEEDED(hr)) {
			hr = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF:CornflowerBlue), &pCornflowerBlueBrush);
		}
	}
	
	return hr;
}

void DiscardGraphicsResources() {
	SafeRelease(&pRenderTarget);
	SafeRelease(&pLightSlateGrayBrush);
	SafeRelease(&pCornflowerBlueBrush);
}

// WindowProc 函数原型
LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(
    HINSTANCE /* hInstance */,
    HINSTANCE /* hPrevInstance */,
    LPSTR /* lpCmdLine */,
    int /* nCmdShow */
    )
{
	HRESULT hr;
	
}