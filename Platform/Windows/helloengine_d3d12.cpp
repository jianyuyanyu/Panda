// include the basic windows header file
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <tchar.h>
#include <stdint.h>

#include <d3d12.h>
#include "d3dx12.h"
#include <DXGI1_4.h>
#include <D3DCompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>

#include <wrl/client.h>

#include <string>
#include <exception>

namespace Panda {
	// Helper class for COM exceptions
	class com_exception : public std::exception {
	public:
		com_exception(HRESULT hr) : result(hr) {
		}
		
		virtual const char* what() const override {
			static char s_str[64] = {0};
			sprintf_s(s_str, "Failure with HRESULT of %08X", 
				static_cast<unsigned int>(result));
			return s_str;
		}
		
	private:
		HRESULT result;
	};
	
	// Helper utility converts D3D API failures into exceptions.
	inline void ThrowIfFailed (HRESULT hr) {
		if (FAILED(hr)) {
			throw com_exception(hr);
		}
};

using namespace Panda;
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;
using namespace std;

const uint32_t SCREEN_WIDTH  =  960;
const uint32_t SCREEN_HEIGHT =  480;

const uint32_t nFrameCount = 2;

const bool bUseWarpDevice = true;

// global declarations
D3D12_VIEWPORT					g_ViewPort = {0.0f, 0.0f, 
											  static_cast<float> (nScreenWidth), static_cast<float>(nScreenHeight)) };		// viewport structure
D3D12_RECT						g_ScissorRect = {0, 0, nScreenWidth, nScreenHeight};	// scissor rect structure
ComPtr<IDXGISwapChain3> 		g_pSwapChain = nullptr;			// the pointer to the swap chain interface 
ComPtr<ID3D12Device>			g_pDev = nullptr;				// the pointer to out Direct3D device interface
ComPtr<ID3D12Resource>			g_pRenderTargets[nFrameCount];	// the pointer to rendering buffer. [decriptor]
ComPtr<ID3D12CommandAllocator>	g_pCommandAllocator;			// the pointer to command buffer allocator
ComPtr<ID3D12CommandQueue>		g_pCommandQueue;				// the pointer to command queue
ComPtr<ID3D12RootSignature>		g_pRootSignature;				// a graphics root signature defines what resources are bound to the pipeline
ComPtr<ID3D12DescriptorHeap>	g_pRtvHeap;						// an array of descriptors of GPU objects
ComPtr<ID3D12PipelineState>		g_pPipelineState;				// an object maintains the state of all currently set shaders
																// and certain fixed function state objects
																// such as the input assembler, tesselator, rasteriazer and output manager
ComPtr<ID3D12CommandList>		g_pCommandList;					// a list to store CPU commands, which will be submmited to GPU to execute

uint32_t g_nRtvDescriptorSize;

ComPtr<ID3D12Resource>			g_pVertexBuffer;				// the pointer to the vertex buffer
D3D12_VERTEX_BUFFER_VIEW		g_VertexBufferView;				// a view of the vertex buffer

// Synchronization objects
uint32_t			g_nFrameIndex;
HANDLE				g_hFenceEvent;
ComPtr<ID3D12Fence>	g_pFence;
uint32_t			g_nFenceValue;

// vertex buffer structure
struct VERTEX {
        XMFLOAT3    Position;
        XMFLOAT4    Color;
};

wstring g_AssetPath;

// Helper function for resolving the full path of assets
std::wstring GetAssetFullPath(LPCWSTR assetName) {
	return g_AssetPath + assetName;
}

void GetAssetsPath(WCHAR* path, UINT pathSize) {
	if (path == nullptr) {
		throw std::exception();
	}
	
	DWORD size = GetModuleFileNameW(nullptr, path, pathSize);
	if (size == 0 || size == pathSize) {
		// Method failed or path was truncated.
		throw std::exception();
	}
	
	WCHAR* lastSlash = wcsrcnr(path, L'\\');
	if (lastSlash) { 
		*(lastSlash + 1) = L'\0';
	}
}

void CreateRenderTarget() {
    HRESULT hr;
    ID3D11Texture2D *pBackBuffer;

    // Get a pointer to the back buffer
    g_pSwapchain->GetBuffer( 0, __uuidof( ID3D11Texture2D ),
                                 ( LPVOID* )&pBackBuffer );

    // Create a render-target view
    g_pDev->CreateRenderTargetView( pBackBuffer, NULL,
                                          &g_pRTView );
    pBackBuffer->Release();

    // Bind the view
    g_pDevcon->OMSetRenderTargets( 1, &g_pRTView, NULL );
}

void SetViewPort() {
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = SCREEN_WIDTH;
    viewport.Height = SCREEN_HEIGHT;

    g_pDevcon->RSSetViewports(1, &viewport);
}

// this is the function that loads and prepares the shaders
void InitPipeline() {
    // load and compile the two shaders
    ID3DBlob *VS, *PS;

    D3DReadFileToBlob(L"copy.vso", &VS);
    D3DReadFileToBlob(L"copy.pso", &PS);

    // encapsulate both shaders into shader objects
    g_pDev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &g_pVS);
    g_pDev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &g_pPS);

    // set the shader objects
    g_pDevcon->VSSetShader(g_pVS, 0, 0);
    g_pDevcon->PSSetShader(g_pPS, 0, 0);

    // create the input layout object
    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    g_pDev->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &g_pLayout);
    g_pDevcon->IASetInputLayout(g_pLayout);

    VS->Release();
    PS->Release();
}

// this is the function that creates the shape to render
void InitGraphics() {
    // create a triangle using the VERTEX struct
    VERTEX OurVertices[] =
    {
        {XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
        {XMFLOAT3(0.45f, -0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
        {XMFLOAT3(-0.45f, -0.5f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)}
    };


    // create the vertex buffer
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
    bd.ByteWidth = sizeof(VERTEX) * 3;             // size is the VERTEX struct * 3
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

    g_pDev->CreateBuffer(&bd, NULL, &g_pVBuffer);       // create the buffer

    // copy the vertices into the buffer
    D3D11_MAPPED_SUBRESOURCE ms;
    g_pDevcon->Map(g_pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
    memcpy(ms.pData, OurVertices, sizeof(VERTEX) * 3);                       // copy the data
    g_pDevcon->Unmap(g_pVBuffer, NULL);                                      // unmap the buffer
}

// this function prepare graphic resources for use
HRESULT CreateGraphicsResources(HWND hWnd)
{
    HRESULT hr = S_OK;
    if (g_pSwapchain == nullptr)
    {
		#if defined(_DEBUG)
        // Enable the D3D12 debug layer.
        {
            ComPtr<ID3D12Debug> debugController;
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
            {
                debugController->EnableDebugLayer();
            }
        }
		#endif
		
		// 1. We need to know which function our device supported. So, we use DXGI library.
	}
    return hr;
}

void DiscardGraphicsResources()
{
    SafeRelease(&g_pLayout);
    SafeRelease(&g_pVS);
    SafeRelease(&g_pPS);
    SafeRelease(&g_pVBuffer);
    SafeRelease(&g_pSwapchain);
    SafeRelease(&g_pRTView);
    SafeRelease(&g_pDev);
    SafeRelease(&g_pDevcon);
}

// this is the function used to render a single frame
void RenderFrame()
{
    // clear the back buffer to a deep blue
    const FLOAT clearColor[] = {0.0f, 0.2f, 0.4f, 1.0f};
    g_pDevcon->ClearRenderTargetView(g_pRTView, clearColor);

    // do 3D rendering on the back buffer here
    {
        // select which vertex buffer to display
        UINT stride = sizeof(VERTEX);
        UINT offset = 0;
        g_pDevcon->IASetVertexBuffers(0, 1, &g_pVBuffer, &stride, &offset);

        // select which primtive type we are using
        g_pDevcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // draw the vertex buffer to the back buffer
        g_pDevcon->Draw(3, 0);
    }

    // swap the back buffer and the front buffer
    g_pSwapchain->Present(0, 0);
}

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd,
                         UINT message,
                         WPARAM wParam,
                         LPARAM lParam);

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR lpCmdLine,
                   int nCmdShow)
{
    // the handle for the window, filled by a function
    HWND hWnd;
    // this struct holds information for the window class
    WNDCLASSEX wc;
	
	WCHAR assetsPath[512];
	GetAssetsPath(assetsPath, _countof(assetsPath));
	g_AssetPath = assetsPath;

    // clear out the window class for use
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    // fill in the struct with the needed information
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = _T("WindowClassd3d12");

    // register the window class
    RegisterClassEx(&wc);

    // create the window and use the result as the handle
    hWnd = CreateWindowEx(0,
                          _T("WindowClassd3d12"),                   // name of the window class
                          _T("Hello, Engine![Direct 3D 12]"),      // title of the window
                          WS_OVERLAPPEDWINDOW,                  // window style
                          100,                                  // x-position of the window
                          100,                                  // y-position of the window
                          nScreenWidth,                         // width of the window
                          nScreenHeight,                        // height of the window
                          NULL,                                 // we have no parent window, NULL
                          NULL,                                 // we aren't using menus, NULL
                          hInstance,                            // application handle
                          NULL);                                // used with multiple windows, NULL

    // display the window on the screen
    ShowWindow(hWnd, nCmdShow);

    // enter the main loop:

    // this struct holds Windows event messages
    MSG msg;

    // wait for the next message in the queue, store the result in 'msg'
    while(GetMessage(&msg, nullptr, 0, 0))
    {
        // translate keystroke messages into the right format
        TranslateMessage(&msg);

        // send the message to the WindowProc function
        DispatchMessage(&msg);
    }

    // return this part of the WM_QUIT message to Windows
    return msg.wParam;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    bool wasHandled = false;

    // sort through and find what code to run for the message given
    switch(message)
    {
	case WM_CREATE:
		wasHandled = true;
        break;	

	case WM_PAINT:
		result = CreateGraphicsResources(hWnd);
        RenderFrame();
		wasHandled = true;
        break;

	case WM_SIZE:
		if (g_pSwapchain != nullptr)
		{
		    DiscardGraphicsResources();
			g_pSwapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		}
		wasHandled = true;
        break;

	case WM_DESTROY:
		DiscardGraphicsResources();
		PostQuitMessage(0);
		wasHandled = true;
        break;

    case WM_DISPLAYCHANGE:
        InvalidateRect(hWnd, nullptr, false);
        wasHandled = true;
        break;
    }

    // Handle any messages the switch statement didn't
    if (!wasHandled) { result = DefWindowProc (hWnd, message, wParam, lParam); }
    return result;
}