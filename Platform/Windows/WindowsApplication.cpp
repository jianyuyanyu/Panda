#include <tchar.h>
#include "WindowsApplication.hpp"

using namespace Panda;

int Panda::WindowsApplication::Initialize()
{
    int result;

    result = BaseApplication::Initialize();

    if (result != 0)
        exit(result);

    // 获取控制台的句柄
    HINSTANCE hInstance = GetModuleHandle(NULL);

    // 窗口类
    WNDCLASSEX wc;

    // 清空窗口类对象
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    // 填充窗口类
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = _T("Panda");

    // 注册窗口类
    RegisterClassEx(&wc);

    // 创建窗口
    m_hWnd = CreateWindowEx(0,
                          "Panda",      					// 窗口类名
                          m_Config.appName,             	// 窗口名
                          WS_OVERLAPPEDWINDOW,              // 窗口风格
                          CW_USEDEFAULT,                    // 窗口的x坐标
                          CW_USEDEFAULT,                    // 窗口的y坐标
                          m_Config.screenWidth,             // 窗口的宽度
                          m_Config.screenHeight,            // 窗口的高度
                          NULL,                             // 父窗口
                          NULL,                             // 菜单
                          hInstance,                        // 应用句柄
                          NULL);                            // 不使用多窗口

    // 显示窗口
    ShowWindow(m_hWnd, SW_SHOW);

    return result;
}

void Panda::WindowsApplication::Finalize()
{
}

void Panda::WindowsApplication::Tick()
{
    // Windows消息结构
    MSG msg;

    // 使用PeekMessage代替GetMessage的目的是不阻塞线程
    // 除非是引擎正在启动的时候
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);

        DispatchMessage(&msg); 
    }
}

// 消息处理
LRESULT CALLBACK Panda::WindowsApplication::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   WindowsApplication* pThis;
    if (message == WM_NCCREATE)
    {
        pThis = static_cast<WindowsApplication*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);

        SetLastError(0);
        if (!SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis)))
        {
            if (GetLastError() != 0)
                return FALSE;
        }
    }
    else
    {
        pThis = reinterpret_cast<WindowsApplication*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    // sort through and find what code to run for the message given
    switch(message)
    {
        case WM_PAINT:
            {
                g_pApp->OnDraw();
            }
            break;
        case WM_KEYDOWN:
            {
                // we will replace this with input manager
                //m_Quit = true;
            } 
            break;

            // this message is read when the window is closed
        case WM_DESTROY:
            {
                // close the application entirely
                PostQuitMessage(0);
                m_Quit = true;
            }
    }

    // Handle any messages the switch statement didn't
    return DefWindowProc (hWnd, message, wParam, lParam);
}
