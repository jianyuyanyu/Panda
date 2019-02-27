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

const uint32_t nScreenWidth  =  960;
const uint32_t nScreenHeight =  480;

const uint32_t nFrameCount = 2;

const bool bUseWarpDevice = true;

// global declarations
D3D12_VIEWPORT					g_ViewPort = {0.0f, 0.0f, 
											  static_cast<float> (nScreenWidth), static_cast<float>(nScreenHeight) };		// viewport structure
D3D12_RECT						g_ScissorRect = {0, 0, nScreenWidth, nScreenHeight};	// scissor rect structure
ComPtr<IDXGISwapChain3> 		g_pSwapChain = nullptr;			// the pointer to the swap chain interface 
ComPtr<ID3D12Device>			g_pDevice = nullptr;			// the pointer to out Direct3D device interface
ComPtr<ID3D12Resource>			g_pRenderTargets[nFrameCount];	// the pointer to rendering buffer. [decriptor]
ComPtr<ID3D12CommandAllocator>	g_pCommandAllocator;			// the pointer to command buffer allocator
ComPtr<ID3D12CommandQueue>		g_pCommandQueue;				// the pointer to command queue
ComPtr<ID3D12RootSignature>		g_pRootSignature;				// a graphics root signature defines what resources are bound to the pipeline
ComPtr<ID3D12DescriptorHeap>	g_pRtvHeap;						// an array of descriptors of GPU objects
ComPtr<ID3D12PipelineState>		g_pPipelineState;				// an object maintains the state of all currently set shaders
																// and certain fixed function state objects
																// such as the input assembler, tesselator, rasteriazer and output manager
ComPtr<ID3D12GraphicsCommandList>		g_pCommandList;					// a list to store CPU commands, which will be submmited to GPU to execute

uint32_t g_nRtvDescriptorSize;

ComPtr<ID3D12Resource>			g_pVertexBuffer;				// the pointer to the vertex buffer
D3D12_VERTEX_BUFFER_VIEW		g_VertexBufferView;				// a view of the vertex buffer

// Synchronization objects
uint32_t			g_nFrameIndex;
HANDLE				g_hFenceEvent;
ComPtr<ID3D12Fence>	g_pFence;
uint32_t			g_nFenceValue;

D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS	g_msQualityLevels;	// multi-sample support information
HWND g_hWnd;	// window handle
ComPtr<IDXGIFactory4> g_pDXGIFactory;	// dxgi factory

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
	
	WCHAR* lastSlash = wcsrchr(path, L'\\');
	if (lastSlash) { 
		*(lastSlash + 1) = L'\0';
	}
}


// this is the function that loads and prepares the shaders
void InitPipeline() {
	// create an empty root signature
	CD3DX12_ROOT_SIGNATURE_DESC rsd;
	rsd.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	
	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	
	ThrowIfFailed(D3D12SerializeRootSignature(&rsd, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
	ThrowIfFailed(g_pDev->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&g_pRootSignature)));
	
	// load the shaders
#if defined (_DEBUG)
		// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif
	ComPtr<ID3DBlob> vertexShader;
	ComPtr<ID3DBlob> pixelShader;
	
	D3DCompileFromFile (
		GetAssetFullPath(L"copy.vs").c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_5_0",
		compileFlags,
		0,
		&vertexShader,
		&error);
	if (error) { 
		OutputDebugString ((LPCTSTR)error->GetBufferPointer()); 
		error->Release(); 
		throw std::exception(); 
	}
	
	D3DCompileFromFile(
		GetAssetFullPath(L"copy.ps").c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		compileFlags,
		0,
		&pixelShader,
		&error);
	if (error) {
		OutputDebugString((LPCTSTR)error->GetBufferPointer());
		error->Release();
		throw std::exception();
	}
	
	// create the input layout object
	D3D12_INPUT_ELEMENT_DESC ied[] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};
	
	// describe and create the graphics pipeline state object (PSO)
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psod = {};
	psod.InputLayout = {ied, _countof(ied)};
	psod.pRootSignature = g_pRootSignature.Get();
	psod.VS = {reinterpret_cast<UINT8*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
	psod.PS = { reinterpret_cast<UINT8*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
	psod.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psod.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psod.DepthStencilState.DepthEnable = FALSE;
	psod.DepthStencilState.StencilEnable = FALSE;
	psod.SampleMask = UINT_MAX;
	psod.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psod.NumRenderTargets = 1;
	psod.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psod.SampleDesc.Count = 1;
	ThrowIfFailed(g_pDev->CreateGraphicsPipelineState (&psod, IID_PPV_ARGS(&g_pPipelineState)));
	
	// create command list
	ThrowIfFailed(g_pDev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_pCommandAllocator)));
	ThrowIfFailed(g_pDev->CreateCommandList(0,
					D3D12_COMMAND_LIST_TYPE_DIRECT,
					g_pCommandAllocator.Get(),
					g_pPipelineState.Get(),
					IID_PPV_ARGS(&g_pCommandList)));
					
	ThrowIfFailed(g_pCommandList->Close());
}

void WaitForPreviousFrame() {
	// WAITING FOR THE FRAME TO COMPLET BEFORE CONTINUING IS NOT BEST PRACTICE.
	// This is code implemented as such for simplicity. More advanced ssamples
	// illustrate how to use fences for efficient resource usage.
	
	// Single and increment the fence value.
	const uint64_t fence = g_nFenceValue;
	ThrowIfFailed(g_pCommandQueue->Signal(g_pFence.Get(), fence));
	g_nFenceValue++;
	
	// Wait untile the previous frame is finished.
	if (g_pFence->GetCompletedValue() < fence) {
		ThrowIfFailed(g_pFence->SetEventOnCompletion(fence, g_hFenceEvent));
		WaitForSingleObject(g_hFenceEvent, INFINITE);
	}
	
	g_nFrameIndex = g_pSwapChain->GetCurrentBackBufferIndex();
}

// this is the function that creates the shape to render
void InitGraphics() {
    // create a triangle using the VERTEX struct
    VERTEX OurVertices[] =
    {
        {XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
        {XMFLOAT3(0.45f, -0.5, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
        {XMFLOAT3(-0.45f, -0.5f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)}
    };
	
	const UINT vertexBufferSize = sizeof(OurVertices);

	// Note: using upload heaps to transfer static data like vert buffers is not
	// recommanded. Every time the GPU needs it, the upload heap will be marshalled
	// over. Please read up on Default Heap usage. An upload heap is used here for 
	// code simplicity and becaurse there are very few verts to actually transfer.
	ThrowIfFailed(g_pDev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&g_pVertexBuffer)));
		
	// copy the vertices into the buffer
	uint8_t* pVertexDataBegin;
	CD3DX12_RANGE readRange(0, 0);	// we do not intend to read this buffer on CPU 
	ThrowIfFailed(g_pVertexBuffer->Map(0, &readRange,
					reinterpret_cast<void**>(&pVertexDataBegin)));	// map the buffer
	memcpy(pVertexDataBegin, OurVertices, vertexBufferSize);	// copy the data
	g_pVertexBuffer->Unmap(0, nullptr);
	
	// initialize the vertex buffer size
	g_VertexBufferView.BufferLocation = g_pVertexBuffer->GetGPUVirtualAddress();
	g_VertexBufferView.StrideInBytes = sizeof(VERTEX);
	g_VertexBufferView.SizeInBytes = vertexBufferSize;
	
	// create synchronization objects and wait until assets have been unloaded to the GPU
	ThrowIfFailed(g_pDev->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_pFence)));
	g_nFenceValue = 1;
	
	// create an event handle to use for synchronization
	g_hFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (g_hFenceEvent == nullptr) {
		ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
	}
	
	// wait for the command list to execute; we are reusing the same command
	// list in our main loop but for now, we just wait for setup to 
	// complete before continuing.
	WaitForPreviousFrame();
}



void DiscardGraphicsResources()
{
	WaitForPreviousFrame();
	CloseHandle(g_hFenceEvent);
}

void PopulateCommandList() {
	// command list allocators can only be reset when the associated
	// command lists have finished execution on the GPU; apps should use
	// fences to determine GPU execution progress.
	ThrowIfFailed(g_pCommandAllocator->Reset());
	
	// however, when ExecuteCommandLists() is called on a particular command
	// list, that command list can then be reset at any time and muset be before
	// re-recording.
	ThrowIfFailed(g_pCommandList->Reset(g_pCommandAllocator.Get(), g_pPipelineState.Get()));
	
	// set necessary state
	g_pCommandList->SetGraphicsRootSignature(g_pRootSignature.Get());
	g_pCommandList->RSSetViewports(1, &g_ViewPort);
	g_pCommandList->RSSetScissorRects(1, &g_ScissorRect);
	
	// Indicate that the back buffer will be used as a render target.
	g_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			g_pRenderTargets[g_nFrameIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET));
			
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(g_pRtvHeap->GetCPUDescriptorHandleForHeapStart(), g_nFrameIndex, g_nRtvDescriptorSize);
	g_pCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
	
	// clear the back buffer to a deep blue
	const FLOAT clearColor[] = {0.0f, 0.2f, 0.4f, 1.0f};
	g_pCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	
	// do 3D rendering on the back buffer here
	{
		// select with vertex buffer to display
		g_pCommandList->IASetVertexBuffers(0, 1, &g_VertexBufferView);
		
		// select which primtive type we are using
		g_pCommandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		// draw the vertex buffer to the back buffer
		g_pCommandList->DrawInstanced(3, 1, 0, 0);
	}
	
	// Indicate that the back buffer will now be used to present.
	g_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			g_pRenderTargets[g_nFrameIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT));
			
	ThrowIfFailed(g_pCommandList->Close());
}

// this is the function used to render a single frame
void RenderFrame()
{
	// record all the commands we need to render the scene into the command list
	PopulateCommandList();
	
	// execute the command list
	ID3D12CommandList* ppCommandLists[] = {g_pCommandList.Get() };
	g_pCommandQueue->ExecuteCommandLists (_countof(ppCommandLists), ppCommandLists);
	
	// swap the back buffer and the front buffer
	ThrowIfFailed(g_pSwapChain->Present(1, 0));
	
	WaitForPreviousFrame();
}

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
	ThrowIfFailed(g_pDevice-CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&g_pCommandQueue)));
	
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

voiud CreateDescriptors() {
}



void InitDirect3D12() {
	CreateDevice();
	
	// We them here?
	ThrowIfFailed(g_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_pFence)));
	mRtvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCbvSrvUavDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	CreateCommandObjects();
	
	CreateSwapChain();
	
	CreateDescriptors();
}

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

bool InitMainWindow(HINSTANCE hInstance, int nCmdShow);

// Message loop
int Run();
						 
// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR lpCmdLine,
                   int nCmdShow)
{
	WCHAR assetsPath[512];
	GetAssetsPath(assetsPath, _countof(assetsPath));
	g_AssetPath = assetsPath;

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
		MessageBox(0, L"Create window failed.", 0, 0);
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

void OnResize() {
	
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

	case WM_PAINT:
		CreateGraphicsResources(hWnd);
        RenderFrame();
		wasHandled = true;
        break;

	case WM_SIZE:
		OnResize();
		// if (g_pSwapChain != nullptr)
		// {
		    // DiscardGraphicsResources();
			// g_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		// }
		wasHandled = true;
        break;

	case WM_DESTROY:
		DiscardGraphicsResources();
		PostQuitMessage(0);
		wasHandled = true;
        break;
    }

    // Handle any messages the switch statement didn't
    if (!wasHandled) { result = DefWindowProc (hWnd, message, wParam, lParam); }
    return result;
}