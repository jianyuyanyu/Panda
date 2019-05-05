// 包含windows头文件
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>

// WindowProc函数原型
LRESULT CALLBACK WindowProc(HWND hWnd,
                         UINT message,
                         WPARAM wParam,
                         LPARAM lParam);

// Windows应用的入口函数
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR lpCmdLine,
                   int nCmdShow)
{
  // 窗口句柄，由函数生成
    HWND hWnd;
    // 保存窗口类的信息
    WNDCLASSEX wc;

    // 初始化数据
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    // 使用需要的数据填充类结构
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = _T("WindowClass1");

    // 注册窗口类
    RegisterClassEx(&wc);

    // 创建窗口，保存句柄
    hWnd = CreateWindowEx(0,
                          _T("WindowClass1"),    // 窗口名
                          _T("Hello, Engine!"),   // 窗口标题
                          WS_OVERLAPPEDWINDOW,    // 窗口格式
                          300,    // 窗口x坐标
                          300,    // 窗口y坐标
                          500,    //窗口宽度
                          400,    // 窗口高度
                          NULL,    // 父窗口
                          NULL,    // 菜单项
                          hInstance,    // 实例句柄
                          NULL);    // 多窗口应用

    // 显示窗口
    ShowWindow(hWnd, nCmdShow);

    // 进入主循环

    // 这个结构提保存了Windows事件消息
    MSG msg;

    // 等待下一个消息，保存到msg结构中
    while(GetMessage(&msg, NULL, 0, 0))
    {
        // 转换按键消息到合适的结构
        TranslateMessage(&msg);

        // 发送消息到WindowProc函数
        DispatchMessage(&msg);
    }

    // 返回WM_QUIT的部分内容给Windows
    return msg.wParam;
}

// 这是消息处理函数
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // 排序，找到合适的处理过程
    switch(message)
    {
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			
			HDC hdc = BeginPaint(hWnd, &ps);
			RECT rect = {20, 20, 60, 80};
			HBRUSH brush = (HBRUSH)GetStockObject(BLACK_BRUSH);
			
			FillRect(hdc, &rect, brush);
			
			EndPaint(hWnd, &ps);
		}
			break;
			
        // 窗口关闭的时候这个消息会处理
        case WM_DESTROY:
            {
                // 关闭整个应用
                PostQuitMessage(0);
                return 0;
            } 
			break;
    }

    // 处理剩余的消息
    return DefWindowProc (hWnd, message, wParam, lParam);
}

