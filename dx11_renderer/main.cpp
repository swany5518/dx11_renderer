#include <iostream>
#include <windowsx.h>
#include <vector>
#include <d3dcompiler.h>

#include "renderer.h"
#include "widgets.h"
#include "widget_list.h"

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dwmapi.lib")
#include <dwmapi.h>

// define the screen resolution
#define SCREEN_WIDTH  1200
#define SCREEN_HEIGHT 1200

widget_list widgets{};

std::wstring get_computer_unique_classname()
{
    TCHAR ComputerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD dwNameLength = MAX_COMPUTERNAME_LENGTH + 1;
    GetComputerNameW(ComputerName, &dwNameLength);

    //for (auto i = 0; i < 15; ++i)
        //ComputerName[i] -= 0x3;

    return std::wstring(ComputerName, dwNameLength);
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    auto get_window_pos = [hWnd]() -> vec2
    {
        RECT wnd_pos{};
        GetWindowRect(hWnd, &wnd_pos);
        return vec2{ static_cast<float>(wnd_pos.left), static_cast<float>(wnd_pos.top) };
    };

    static vec2 wnd_pos;
    
    switch (message)
    {
    case WM_CREATE:
    case WM_MOVE:
    case WM_SIZE:
        wnd_pos = get_window_pos();
        break;
    case WM_MOUSEMOVE:
        //std::cout << "WM_MOUSEMOVE " << GET_X_LPARAM(lParam) << std::endl;
        widgets.add_input_msg(widget_input{ vec2{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)} });
        break;
    case WM_LBUTTONDOWN:
        SetCapture(hWnd);
        widgets.add_input_msg(widget_input{ input_type::lbutton_down, vec2{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)} });
        break;
    case WM_LBUTTONUP:
        //std::cout << "WM_LBUTTONUP" << std::endl;
        ReleaseCapture();
        widgets.add_input_msg(widget_input{ input_type::lbutton_up, vec2{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)} });
        break;
    case WM_CHAR:
        widgets.add_input_msg(static_cast<char>(wParam));
        break;
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    } break;
    case WM_SETCURSOR:
        //std::cout << "WM_SETCURSOR" << std::endl;
        break;
    case WM_NCHITTEST:
        //std::cout << "WM_HITTEST" << std::endl;
        break;
    case WM_MOUSELEAVE:
        //std::cout << "WM_MOUSELEAVE" << std::endl;
        break;
    case WM_NCMOUSELEAVE:
        //std::cout << "WM_NCMOUSELEAVE" << std::endl;
        break;
    default:
        //std::cout << "WM: " << message << std::endl;
        break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE);
    AllocConsole();
    freopen("conin$" , "r", stdin);
    freopen("conout$", "w", stdout);
    freopen("conout$", "w", stderr);

    HWND hwnd;
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = DefWindowProcA;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"WindowClass";

    RegisterClassEx(&wc);

    RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    //hwnd = CreateWindowEx(NULL, L"WindowClass", L"renderer example", WS_OVERLAPPEDWINDOW, 300, 300, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, hInstance, NULL);
    hwnd = (HWND)0x50f78;
    ShowWindow(hwnd, SW_SHOW);
    //hWnd = FindWindowA(NULL, "Windows PowerShell");
    //ShowWindow(hWnd, nCmdShow);
    // hWnd = FindWindowW(get_computer_unique_classname().c_str(), NULL);
    // const UINT opacity_flag = 0x02;
    // const UINT color_key = 0x000000;
    // const UINT opacity = 0xFF;
    // SetLayeredWindowAttributes(hWnd, color_key, opacity, opacity_flag);

    //HWND hwnd = FindWindowA("CaptureTarget", NULL);
    //UpdateWindow(hwnd);
    //ShowWindow(hwnd, SW_SHOW);

    //SetWindowLongA(hwnd, GWL_STYLE, (WS_VISIBLE | WS_POPUP) & WS_VSCROLL);
    //SetWindowLongA(hwnd, GWL_EXSTYLE, GetWindowLongA(hwnd, GWL_EXSTYLE) | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED);
    //SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE |SWP_NOSIZE |SWP_NOZORDER);
    //
    //MARGINS margin;
    //margin.cyBottomHeight = -1;
    //margin.cxLeftWidth = -1;
    //margin.cxRightWidth = -1;
    //margin.cyTopHeight = -1;
    //DwmExtendFrameIntoClientArea(hwnd, &margin);
    //
    const UINT opacity_flag = LWA_ALPHA | LWA_COLORKEY;
    const UINT color_key = 0x000000;
    const UINT opacity = 0xFF;
    SetLayeredWindowAttributes(hwnd, color_key, opacity, opacity_flag);
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);

    renderer renderer{};
    renderer.initialize(hwnd, { 0.f, 0.f, 0.f, 0.f }, L"Back To 1982");
    renderer.set_render_target_color({ 0.0f, 0.0f, 0.0f, 0.0f });
    widget::set_renderer(&renderer);

    color red{ 1.f, 0.f, 0.f, 1.f };
    color green{ 0.f, 1.f, 0.f, 1.f };
    color blue{ 0.f, 0.f, 1.f, .5f };
    color black{ 0.f, 0.f, 0.f, 1.f };
    color white{ 1.f, 1.f, 1.f, 1.f };
    color yellow{ 1.f, 1.f, 0.f, 1.f };

    vec2 points[] =
    {
        {100, 650},
        {150, 750},
        {200, 680},
        {300, 765},
        {400, 720}
    };

    MSG msg_;

    auto time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
    int frame_count = 0;
    int last_second_frames = 0;

    while (TRUE)
    {
        if (PeekMessage(&msg_, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg_);
            DispatchMessage(&msg_);

            if (msg_.message == WM_QUIT)
                break;
        }
        
        static float x = 0.f;
        renderer.add_cornered_frame({ fmodf(x++, 300.f), 100.f }, { 200.f, 200.f }, red, black, .25f, .25f, 5.f, 2.f);
       
        renderer.draw();
    }
    
    //std::cin.get();
    renderer.cleanup();
   
}


int main()
{ 
    uintptr_t gm = 0x6EFB049634016553;

    /*ID3DBlob* blob = nullptr;
    D3DCompile(shaders::pixel, sizeof(shader::shader), nullptr, nullptr, nullptr, "PS", "ps_4_0", 0, 0, &blob, nullptr);
    size_t size = blob->GetBufferSize();

    std::vector<uint8_t> bytes{};
    bytes.resize(size);

    memcpy(bytes.data(), blob->GetBufferPointer(), size);

    int i = 0;
    for (auto b : bytes)
    {
        std::cout << std::hex << std::uppercase << (b < 16 ? "0x0" : "0x") << b + 0 << (i == size ? "" : ", ");

        if (i++ > 0 && i % 30 == 0)
            std::cout << std::endl;
    }

    blob->Release();*/
}