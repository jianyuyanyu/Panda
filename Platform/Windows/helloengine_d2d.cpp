#include <windows.h>
#include <windowsx.h>
#include <tchar.h>

#include <d2d1.h>

ID2D1Factory				*pFactory = nullptr;
ID2D1HwndRenderTarget		*pRenderTarget = nullptr;
ID2D1SolidColorBrush		*pLightSlateGrayBrush = nullptr;
ID2D1SolidColorBrush		*pCornflowerBlueBrush = nullptr;

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
			hr = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::CornflowerBlue), &pCornflowerBlueBrush);
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

int WINAPI WinMain(HINSTANCE  hInstance ,
					HINSTANCE  hPrevInstance ,
					LPSTR  lpCmdLine ,
					int  nCmdShow)
{
	WNDCLASSEX wc;
	
	// 初始化COM
	if(FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))) return -1;
	
	// 清理窗口类
	ZeroMemory(&wc, sizeof (WNDCLASSEX));
	
	wc.cbSize = sizeof (WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = _T("WindowClass1");
	
	// 注册窗口类
    RegisterClassEx(&wc);
	
	// 创建窗口
	HWND hWnd = CreateWindowEx(0,
								_T("WindowClass1"),							// 窗口类名
								_T("Hello, Engine![Direct 2D]"),			// 窗口标题
								WS_OVERLAPPEDWINDOW,						// 窗口风格
								100,										// 窗口的x坐标
								100,										// 窗口的y坐标
								960,										// 窗口宽度
								540,										// 窗口高度
								NULL,										// 父窗口
								NULL,										// 菜单句柄
								hInstance,									// 应用句柄
								NULL);
								
    // 显示窗口
    ShowWindow(hWnd, nCmdShow);
	
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	CoUninitialize();
	
	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
	bool wasHandled = false;
	
	// 根据消息类型选择处理流程
	switch(message) {
		case WM_CREATE:
		{
			if (FAILED(D2D1CreateFactory (D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory))) {
				result = -1;		// 创建失败
				return result;
			}
			wasHandled = true;
			result = 0;
		}
		break;
		case WM_PAINT:
		{
			HRESULT hr = CreateGraphicsResources(hWnd);
			if (SUCCEEDED(hr)) {
				PAINTSTRUCT ps;
				BeginPaint(hWnd, &ps);
				
				// 开始构建GPU绘制命令
				pRenderTarget->BeginDraw();
				
				// 将背景色刷新成白色
				pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
				
				// 检索绘制区域的大小
				D2D1_SIZE_F rtSize = pRenderTarget->GetSize();
				
				// 绘制格子背景
				int width = static_cast<int> (rtSize.width);
				int height = static_cast<int>(rtSize.height);
		
				for (int x = 0; x < width; x += 10) {
					pRenderTarget->DrawLine(D2D1::Point2F(static_cast<FLOAT>(x), 0.f),
											D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height),
											pLightSlateGrayBrush,
											0.5f
											);
				}
				
				for (int y = 0; y < height; y += 10) {
					pRenderTarget->DrawLine(D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
											D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)),
											pLightSlateGrayBrush,
											0.5f
											);
				}
				
				// 绘制两个矩形
				D2D1_RECT_F rectangle1 = D2D1::RectF(
					rtSize.width / 2 - 50.f,
					rtSize.height / 2 - 50.f,
					rtSize.width / 2 + 50.f,
					rtSize.height / 2 + 50.f
					);
					
				D2D1_RECT_F rectangle2 = D2D1::RectF(
					rtSize.width / 2 - 100.f,
					rtSize.height / 2 - 100.f,
					rtSize.width / 2 + 100.f,
					rtSize.height / 2 + 100.f
				);
				
				// 绘制一个填充的矩形
				pRenderTarget->FillRectangle(&rectangle1, pLightSlateGrayBrush);
				
				// 绘制一个边框
				pRenderTarget->DrawRectangle(&rectangle2, pCornflowerBlueBrush);
				
				// 终止GPU绘制
				hr = pRenderTarget->EndDraw();
				if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
					DiscardGraphicsResources();
				
				EndPaint(hWnd, &ps);
			}
			
			wasHandled = true;
		}
		break;
		case WM_SIZE:
		{
			if (pRenderTarget != nullptr) {
				RECT rc;
				GetClientRect(hWnd, &rc);
				
				D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
				
				pRenderTarget ->Resize(size);
			}
			wasHandled = true;
		}
		break;
		case WM_DESTROY:
		{
			DiscardGraphicsResources();
			if (pFactory) {
				pFactory->Release();
				pFactory = nullptr;
			}
			PostQuitMessage(0);
			result = 0;
			wasHandled = true;
		}
		break;
		case WM_DISPLAYCHANGE:
		{
			InvalidateRect(hWnd, nullptr, false);
			wasHandled = true;
		}
		break;
	}
	
	if (!wasHandled) { result = DefWindowProc (hWnd, message, wParam, lParam);}
	return result;
}