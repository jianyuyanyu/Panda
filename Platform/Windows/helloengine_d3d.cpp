#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <stdint.h>

#include <d3d11.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>

using namespace DirectX;
using namespace DirectX::PackedVector;

const uint32_t SCREEN_WIDTH = 960;
const uint32_t SCREEN_HEIGHT = 480;

// 全局声明
IDXGISwapChain				*g_pSwapchain = nullptr;		// 交换链的接口
ID3D11Device				*g_pDev = nullptr;				// D3D设备接口
ID3D11DeviceContext			*g_pDevcon = nullptr;			// D3D设备上下文

ID3D11RenderTargetView		*g_pRTView = nullptr;

ID3D11InputLayout			*g_pLayout = nullptr;			// 输入布局
ID3D11VertexShader			*g_pVS = nullptr;				// 顶点着色器
ID3D11PixelShader			*g_pPS = nullptr;				// 像素着色器

ID3D11Buffer				*g_pVBuffer = nullptr;			// 顶点缓冲

// 顶点缓冲结构体
struct VERTEX {
	XMFLOAT3		Position;
	XMFLOAT4		Color;
};

template<class T>
inline void SafeRelease(T** ppInterfaceToRelease) {
	if (*ppInterfaceToRelease != nullptr) {
		(*ppInterfaceToRelease) ->Release();
		
		(*ppInterfaceToRelease) = nullptr;
	}
}

void CreateRenderTarget() {
	HRESULT hr;
	ID3D11Texture2D* pBackBuffer;
	
	// 获取一个指针指向背景缓存
	g_pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D),
							(LPVOID*)&pBackBuffer);
							
	// 创建一个渲染目标视图
	g_pDev->CreateRenderTargetView(pBackBuffer, NULL, &g_pRTView);
	
	pBackBuffer->Release();
	
	// 绑定视图
	g_pDevcon->OMSetRenderTargets(1, &g_pRTView, NULL);
}

void SetViewPort() {
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof (D3D11_VIEWPORT));
	
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = SCREEN_WIDTH;
	viewport.Height = SCREEN_HEIGHT;
	
	g_pDevcon->RSSetViewports(1, &viewport);
}

// 这个函数用来加载以及准备着色器
void InitPipeline() {
	// 加载，编译两个着色器
	ID3DBlob *VS;
	ID3DBlob *PS;
	
	D3DReadFileToBlob(L"copy.vso", &VS);
	D3DReadFileToBlob(L"copy.pso", &PS);
	
	// 把所有的着色器都封装到着色器对象中
	g_pDev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &g_pVS);
	g_pDev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &g_pPS);
	
	// 设置着色器对象
	g_pDevcon->VSSetShader(g_pVS, 0, 0);
	g_pDevcon->PSSetShader(g_pPS, 0, 0);
	
	// 创建输入布局对象
	D3D11_INPUT_ELEMENT_DESC ied[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	
	g_pDev->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &g_pLayout);
	g_pDevcon->IASetInputLayout(g_pLayout);
	
	VS->Release();
	PS->Release();
}

// 这个函数用来创建渲染的形状
void InitGraphics() {
	// 使用VERTEX结构体创建三角形
    VERTEX OurVertices[] =
    {
        {XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
        {XMFLOAT3(0.45f, -0.5, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
        {XMFLOAT3(-0.45f, -0.5f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)}
    };
	
	// 创建顶点缓冲区
	D3D11_BUFFER_DESC		bd;
	ZeroMemory(&bd, sizeof(bd));
	
	bd.Usage = D3D11_USAGE_DYNAMIC;	// 写权限。可以被CPU和GPU访问
	bd.ByteWidth = sizeof (VERTEX) * 3;	// 宽度是3个VERTEX
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// 作为顶点缓存使用
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// 允许CPU写入
	
	g_pDev->CreateBuffer(&bd, NULL, &g_pVBuffer);	// 创建缓冲区
	
	// 复制顶点数据到缓冲区内
	D3D11_MAPPED_SUBRESOURCE ms;
	g_pDevcon->Map(g_pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);	// 映射缓冲
	memcpy(ms.pData, OurVertices, sizeof (VERTEX) * 3);
	g_pDevcon->Unmap(g_pVBuffer, NULL);
}

HRESULT CreateGraphicsResources(HWND hWnd) {
	HRESULT hr = S_OK;
	if (g_pSwapchain == nullptr) {
		// 创建保存交换链信息的结构体
		DXGI_SWAP_CHAIN_DESC	scd;
		
		// 清空结构体
		ZeroMemory(&scd, sizeof (DXGI_SWAP_CHAIN_DESC));
		
		// 填充交换链描述结构体
		scd.BufferCount = 1;	// 我们的后备缓冲
		scd.BufferDesc.Width = SCREEN_WIDTH;
		scd.BufferDesc.Height = SCREEN_HEIGHT;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 使用32位颜色
		scd.BufferDesc.RefreshRate.Numerator = 60;
		scd.BufferDesc.RefreshRate.Denominator = 1;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 交换链要如何使用
		scd.OutputWindow = hWnd;	// 使用的窗口
		scd.SampleDesc.Count = 4;	// 多重采样的数量
		scd.Windowed = TRUE;		// 窗口化还是全屏
		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// 允许全屏切换
		
		const D3D_FEATURE_LEVEL FeatureLevels[] = {D3D_FEATURE_LEVEL_11_1,
													D3D_FEATURE_LEVEL_11_0,
													D3D_FEATURE_LEVEL_10_1,
													D3D_FEATURE_LEVEL_10_0,
													D3D_FEATURE_LEVEL_9_3,
													D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1};
		
		D3D_FEATURE_LEVEL FeatureLevelSupported;
		
		HRESULT hr = S_OK;
		
		// 创建设备，设备上下文，和交换链
		hr = D3D11CreateDeviceAndSwapChain(NULL,
										D3D_DRIVER_TYPE_HARDWARE,
										NULL,
										0,
										FeatureLevels,
										_countof(FeatureLevels),
										D3D11_SDK_VERSION,
										&scd,
										&g_pSwapchain,
										&g_pDev,
										&FeatureLevelSupported,
										&g_pDevcon);
										
		if (hr == E_INVALIDARG)  {
			hr = D3D11CreateDeviceAndSwapChain(NULL,
								D3D_DRIVER_TYPE_HARDWARE,
								NULL,
								0,
								&FeatureLevelSupported,
								_countof(FeatureLevels),
								D3D11_SDK_VERSION,
								&scd,
								&g_pSwapchain,
								&g_pDev,
								NULL,
								&g_pDevcon);
		}
		
		if (hr == S_OK)  {
			CreateRenderTarget();
			SetViewPort();
			InitPipeline();
			InitGraphics();
		}
	}
	
	return hr;
}

void DiscardGraphicsResources() {
	SafeRelease(&g_pLayout);
	SafeRelease(&g_pVS);
	SafeRelease(&g_pPS);
	SafeRelease(&g_pVBuffer);
	SafeRelease(&g_pSwapchain);
	SafeRelease(&g_pRTView);
	SafeRelease(&g_pDev);
	SafeRelease(&g_pDevcon);
}

// 这个函数是用来渲染一帧的
void RenderFrame() {
	// 把背景刷成深蓝色
	const FLOAT clearColor[]  = {0.0f, 0.2f, 0.4f, 1.f};
	g_pDevcon->ClearRenderTargetView(g_pRTView, clearColor);
	
	// 做3D渲染到后备缓冲区
	{
		// 选择要显示的顶点缓冲区
		UINT stride = sizeof (VERTEX);
		UINT offset = 0;
		g_pDevcon->IASetVertexBuffers(0, 1, &g_pVBuffer, &stride, &offset);
		
		// 选择要使用的图元类型
		g_pDevcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		// 绘制顶点缓冲区到后备缓冲区
		g_pDevcon ->Draw(3, 0);
	}
	
	// 交换后备缓冲区和前置缓冲区
	g_pSwapchain->Present(0, 0);
}

// WindowProc 函数原型
LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE  hInstance ,
					HINSTANCE  hPrevInstance ,
					LPSTR  lpCmdLine ,
					int  nCmdShow)
{
	WNDCLASSEX wc;
	
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
								_T("Hello, Engine![Direct 3D]"),			// 窗口标题
								WS_OVERLAPPEDWINDOW,						// 窗口风格
								100,										// 窗口的x坐标
								100,										// 窗口的y坐标
								SCREEN_WIDTH,								// 窗口宽度
								SCREEN_HEIGHT,								// 窗口高度
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
	
	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
	bool wasHandled = false;
	
	// 根据消息类型选择处理流程
	switch(message) {
		case WM_CREATE:
		{
			wasHandled = true;
		}
		break;
		case WM_PAINT:
		{
			result = CreateGraphicsResources(hWnd);
			RenderFrame();
			
			wasHandled = true;
		}
		break;
		case WM_SIZE:
		{
			if (g_pSwapchain != nullptr) {
				DiscardGraphicsResources();
			}
			wasHandled = true;
		}
		break;
		case WM_DESTROY:
		{
			DiscardGraphicsResources();
			PostQuitMessage(0);
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