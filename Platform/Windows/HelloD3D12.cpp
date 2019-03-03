// include the basic windows header file
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <tchar.h>
#include <stdint.h>

#include <d3d12.h>
#include "d3dx12.h"
#include <DXGI1_4.h>		// DXGI (DirectX Graphics Infrastructure) is an API used along with Direct3D. The basic idea of 
							// DXGI is that some graphics related tasks are common to multipile graphics APIs. Here are some
							// common functionality, swap chain, display adapters, monitors and supported display modes(
							// resolution, refresh rate), surface formats
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
	}
};

using namespace Panda;
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;
using namespace std;

uint32_t nScreenWidth  =  960;
uint32_t nScreenHeight =  480;

const uint32_t nFrameCount = 2;
uint32_t g_nFrameIndex;

const bool bUseWarpDevice = true;

// global declarations		// viewport structure
ComPtr<IDXGISwapChain> 			g_pSwapChain = nullptr;			// the pointer to the swap chain interface 
ComPtr<ID3D12Device>			g_pDevice = nullptr;			// the pointer to out Direct3D device interface
ComPtr<ID3D12Resource>			g_pRenderTargets[nFrameCount];	// the pointer to rendering buffer. [decriptor]
ComPtr<ID3D12CommandAllocator>	g_pCommandAllocator;			// the pointer to command buffer allocator
ComPtr<ID3D12CommandQueue>		g_pCommandQueue;				// the pointer to command queue
ComPtr<ID3D12DescriptorHeap>	g_pRtvHeap;						// an array of descriptors of GPU objects
ComPtr<ID3D12GraphicsCommandList>		g_pCommandList;					// a list to store CPU commands, which will be submmited to GPU to execute

uint32_t g_nRtvDescriptorSize;

ComPtr<ID3D12Resource>			g_pVertexBuffer;				// the pointer to the vertex buffer
D3D12_VERTEX_BUFFER_VIEW		g_VertexBufferView;				// a view of the vertex buffer

// Synchronization objects
ComPtr<ID3D12Fence>	g_pFence;
uint32_t			g_nFenceValue;

D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS	g_msQualityLevels;	// multi-sample support information
HWND g_hWnd;	// window handle
ComPtr<IDXGIFactory4> g_pDXGIFactory;	// dxgi factory
ComPtr<ID3D12DescriptorHeap>	g_pDsvHeap;		// dsv objects
uint32_t g_rtvDescriptorSize = 0;
uint32_t g_dsvDescriptorSize = 0;
uint32_t g_cbvSrvUavDescriptorSize = 0;
ComPtr<ID3D12Resource>	g_pDepthSetencilBuffer;
D3D12_VIEWPORT g_ScreenViewport; 
D3D12_RECT g_ScissorRect;

void CreateDevice() {
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&g_pDXGIFactory)));
	
	// Try to create hardware device.
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,	// default adapter
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&g_pDevice));
	
	// Fallback to WARP(Windows Advanced Rasterization Platform) device
	if (FAILED(hardwareResult)) {
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(g_pDXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));
		
		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&g_pDevice)));
	}
	
	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render
	// target formats, so we only need to check quality support.
	g_msQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	g_msQualityLevels.SampleCount = 4;
	g_msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	g_msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(g_pDevice->CheckFeatureSupport(
				D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
				&g_msQualityLevels,
				sizeof (g_msQualityLevels)));
}

void CreateCommandObjects() {
	// Command Queue
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(g_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&g_pCommandQueue)));
	
	// Command Allocator and Command List
	ThrowIfFailed(g_pDevice->CreateCommandAllocator(
					D3D12_COMMAND_LIST_TYPE_DIRECT,
					IID_PPV_ARGS(g_pCommandAllocator.GetAddressOf())));
					
	ThrowIfFailed(g_pDevice->CreateCommandList(
					0,
					D3D12_COMMAND_LIST_TYPE_DIRECT,
					g_pCommandAllocator.Get(),	// Assocated command allocator
					nullptr,	// Initial PipelineStateObject
					IID_PPV_ARGS(g_pCommandList.GetAddressOf())));
					
	// Start off in a closed state. This is because the first time we reference
	// to the command list we will Reset it, and it needs to be closed before
	// calling Reset.
	g_pCommandList->Close();
}

void CreateSwapChain() {
	// Release the previous swapchain we will be recreating.
	g_pSwapChain.Reset();
	
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = nScreenWidth;
	sd.BufferDesc.Height = nScreenHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 4;
	sd.SampleDesc.Quality = g_msQualityLevels.NumQualityLevels - 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = nFrameCount;
	sd.OutputWindow = g_hWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	
	// Note: Swap chain uses queue to perform flush
	ThrowIfFailed(g_pDXGIFactory->CreateSwapChain(
					g_pCommandQueue.Get(),
					&sd,
					g_pSwapChain.GetAddressOf()));
}

void CreateDescriptors() {
	// fence
	ThrowIfFailed(g_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_pFence)));
	
	// RTVs
	g_rtvDescriptorSize = g_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = nFrameCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(g_pDevice->CreateDescriptorHeap(
					&rtvHeapDesc,
					IID_PPV_ARGS(g_pRtvHeap.GetAddressOf())));
					
	// DSVs
	g_dsvDescriptorSize = g_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(g_pDevice->CreateDescriptorHeap(
					&dsvHeapDesc,
					IID_PPV_ARGS(g_pDsvHeap.GetAddressOf())));

	// cbv srv uav size
	g_cbvSrvUavDescriptorSize = g_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void InitDirect3D12() {
	CreateDevice();
	
	// We need them here?
	// ThrowIfFailed(g_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_pFence)));
	// mRtvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	// mDsvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	// mCbvSrvUavDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	CreateCommandObjects();
	
	CreateSwapChain();
	
	CreateDescriptors();
}

void FlushCommandQueue() {
	// Advance the fence value to mark commands up to this fence point
	g_nFenceValue++;
	
	// Add an instruction to the command queue to set a new fence point. Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finished
	// processing all the commands prior to this Signal
	ThrowIfFailed(g_pCommandQueue->Signal(g_pFence.Get(), g_nFenceValue));
	
	// Wait until the GPU has completed commands up to this fence point.
	if (g_pFence->GetCompletedValue() < g_nFenceValue) {
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		
		// Fire event when GPU hits current fence
		ThrowIfFailed(g_pFence->SetEventOnCompletion(g_nFenceValue, eventHandle));
		
		// Wait until the GPU hits current fence event is fired
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

bool InitMainWindow(HINSTANCE hInstance, int nCmdShow);

// Message loop
int Run();

// Draw Frame
void Draw();
						 
// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR lpCmdLine,
                   int nCmdShow)
{
	// Create window
	InitMainWindow(hInstance, nCmdShow);
	
	// Initialize DirectX 12
	InitDirect3D12();
	
	return Run();
}

bool InitMainWindow(HINSTANCE hInstance, int nCmdShow) {	
    WNDCLASSEX wc;

    // clear out the window class for use
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    // fill in the struct with the needed information
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = _T("HelloD3D12");
	
	// register the window class
    RegisterClassEx(&wc);
	
    // create the window and use the result as the handle
    g_hWnd = CreateWindowEx(0,
                          _T("HelloD3D12"),                   // name of the window class
                          _T("Hello Direct 12"),      			// title of the window
                          WS_OVERLAPPEDWINDOW,                  // window style
                          100,                                  // x-position of the window
                          100,                                  // y-position of the window
                          nScreenWidth,                         // width of the window
                          nScreenHeight,                        // height of the window
                          NULL,                                 // we have no parent window, NULL
                          NULL,                                 // we aren't using menus, NULL
                          hInstance,                            // application handle
                          NULL);                                // used with multiple windows, NULL

	if (!g_hWnd) {
		MessageBoxW(0, L"Create window failed.", 0, 0);
		return false;
	}
						  
    // display the window on the screen
    ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);
	
	return true;
}

int Run() {
    // this struct holds Windows event messages
    MSG msg;

    // wait for the next message in the queue, store the result in 'msg'
	while(msg.message != WM_QUIT) {
		// If there are window message then process them
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			// translate keystroke messages into the right format
			TranslateMessage(&msg);

			// send the message to the WindowProc function
			DispatchMessage(&msg);
		}
		else {
			Draw();
		}
	}

    // return this part of the WM_QUIT message to Windows
    return msg.wParam;
}

void OnResize() {
	assert(g_pDevice);
	assert(g_pSwapChain);
	assert(g_pCommandAllocator);
	
	// Flush before changing any resources
	FlushCommandQueue();
	
	ThrowIfFailed(g_pCommandList->Reset(g_pCommandAllocator.Get(), nullptr));
	
	// Release the previous resources we will be recreating.
	for (int i = 0; i < nFrameCount; ++i)
		g_pRenderTargets[i].Reset();
	g_pDepthSetencilBuffer.Reset();
	
	// Resize the swap chain
	ThrowIfFailed(g_pSwapChain->ResizeBuffers(
					nFrameCount,
					nScreenWidth, nScreenHeight,
					DXGI_FORMAT_R8G8B8A8_UNORM,
					DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));
	
	g_nFrameIndex = 0;
	
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(g_pRtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < nFrameCount; ++i) {
		ThrowIfFailed(g_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&g_pRenderTargets[i])));
		g_pDevice->CreateRenderTargetView(g_pRenderTargets[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, g_rtvDescriptorSize);
	}
	
	// Create the depth/stencil buffer and view
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = nScreenWidth;
	depthStencilDesc.Height = nScreenHeight;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthStencilDesc.SampleDesc.Count = 4;
	depthStencilDesc.SampleDesc.Quality = g_msQualityLevels.NumQualityLevels - 1;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	
	D3D12_CLEAR_VALUE optClear;
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	ThrowIfFailed(g_pDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(g_pDepthSetencilBuffer.GetAddressOf())));
		
	// Create descriptor to mip level 0 of entire resource using the format of the resource.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice = 0;
	g_pDevice->CreateDepthStencilView(g_pDepthSetencilBuffer.Get(), &dsvDesc, g_pDsvHeap->GetCPUDescriptorHandleForHeapStart());
	
	// Transtion the resource from its initial state to be used as a depth buffer.
	g_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(g_pDepthSetencilBuffer.Get(),
									D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
									
	// Execute the resize commands.
	ThrowIfFailed(g_pCommandList->Close());
	ID3D12CommandList* cmdLists[] = {g_pCommandList.Get()};
	g_pCommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
	
	// Wait until resize is complete.
	FlushCommandQueue();
	
	// Update the viewport transform to cover the client area.
	g_ScreenViewport.TopLeftX = 0;
	g_ScreenViewport.TopLeftY = 0;
	g_ScreenViewport.Width = static_cast<float> (nScreenWidth);
	g_ScreenViewport.Height = static_cast<float> (nScreenHeight);
	g_ScreenViewport.MinDepth = 0.0f;
	g_ScreenViewport.MaxDepth = 1.0f;
	
	g_ScissorRect = {0, 0, static_cast<LONG>(nScreenWidth), static_cast<LONG>(nScreenHeight)};
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    LRESULT result = 0;
    bool wasHandled = false;

    // sort through and find what code to run for the message given
    switch(message)
    {
	case WM_CREATE:
		wasHandled = true;
        break;	

	// case WM_PAINT:
		// wasHandled = true;
        // break;

	case WM_SIZE:
		if (g_pDevice)
			OnResize();
		wasHandled = true;
        break;

	case WM_DESTROY:
		FlushCommandQueue();
		PostQuitMessage(0);
		wasHandled = true;
        break;
    }

    // Handle any messages the switch statement didn't
    if (!wasHandled) { result = DefWindowProc (hWnd, message, wParam, lParam); }
    return result;
}

void Draw() {
	// Reuse the memory assocatied with command recording
	// We can only reset when the associated command lists have finished execution on the GPU
	ThrowIfFailed(g_pCommandAllocator->Reset());
	
	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed (g_pCommandList->Reset(g_pCommandAllocator.Get(), nullptr));
	
	// Indicate a state transition on the resource usage.
	g_pCommandList->ResourceBarrier(1, 
									&CD3DX12_RESOURCE_BARRIER::Transition(
										g_pRenderTargets[g_nFrameIndex].Get(),
										D3D12_RESOURCE_STATE_PRESENT,
										D3D12_RESOURCE_STATE_RENDER_TARGET));
										
	// Set the viewport and scissor rect. This needs to be reset whenever the command list is reset.
	g_pCommandList->RSSetViewports(1, &g_ScreenViewport);
	g_pCommandList->RSSetScissorRects(1, &g_ScissorRect);
	
	// Clear the back buffer and depth buffer
	g_pCommandList->ClearRenderTargetView(
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			g_pRtvHeap->GetCPUDescriptorHandleForHeapStart(),
			g_nFrameIndex,
			g_rtvDescriptorSize),
		Colors::LightSteelBlue,
		0,
		nullptr);
	g_pCommandList->ClearDepthStencilView(
		g_pDsvHeap->GetCPUDescriptorHandleForHeapStart(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f, 0,
		0,
		nullptr);
		
	// Specify the buffers we are going to render to.
	g_pCommandList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			g_pRenderTargets[g_nFrameIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT));
			
	// Done recording commands.
	ThrowIfFailed(g_pCommandList->Close());
	
	// Execute the resize commands.
	ThrowIfFailed(g_pCommandList->Close());
	ID3D12CommandList* cmdLists[] = {g_pCommandList.Get()};
	g_pCommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
	
	// Wait until resize is complete.
	FlushCommandQueue();
}