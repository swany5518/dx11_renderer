#include <iostream>
#include <windowsx.h>
#include <vector>
#include <d3dcompiler.h>

#include "renderer.h"
#include "widgets.h"
#include "widget_list.h"

#pragma comment(lib, "d3dcompiler.lib")

// define the screen resolution
#define SCREEN_WIDTH  1200
#define SCREEN_HEIGHT 1200

widget_list widgets{};

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

void xor_block(uintptr_t start, size_t amt, char* key, size_t key_size)
{
    for (auto i = 0u; i < amt; ++i)
        *reinterpret_cast<char*>(start + i * sizeof(char)) ^= key[i % key_size];
}

int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE);
    AllocConsole();
    freopen("conin$" , "r", stdin);
    freopen("conout$", "w", stdout);
    freopen("conout$", "w", stderr);

    HWND hWnd;
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"WindowClass";

    RegisterClassEx(&wc);

    RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    hWnd = CreateWindowEx(NULL, L"WindowClass", L"renderer example", WS_OVERLAPPEDWINDOW, 300, 300, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);

    renderer renderer{};
    renderer.initialize(hWnd);
    widget::set_renderer(&renderer);

    slider_style test_style{
        text_style{ 
            14.000000, 1.000000,
            { 1.000000, 1.000000, 1.000000, 1.000000 },
            { 0.000000, 0.000000, 0.000000, 1.000000 },
            { 0.000000, 0.000000, 0.000000, 0.000000 } 
        },
        border_style{
            2.000000, 0.000000,
            { 0.000000, 0.000000, 0.000000, 1.000000 },
            { 0.000000, 0.000000, 0.000000, 0.000000 } 
        },
        mc_rect{
            { 0.000000, 0.000000, 0.000000, 0.000000 },
            { 0.000000, 0.000000, 0.000000, 0.000000 },
            { 0.000000, 0.000000, 0.000000, 0.000000 },
            { 0.000000, 0.000000, 0.000000, 0.000000 } 
        },
        mc_rect{
            { 0.500000, 0.500000, 0.500000, 0.500000 },
            { 0.500000, 0.500000, 0.500000, 0.500000 },
            { 0.500000, 0.500000, 0.500000, 0.500000 },
            { 0.500000, 0.500000, 0.500000, 0.500000 } 
        }
    };

    float val = 1.f;
    slider_style style{ text_style{}, border_style{}, mc_rect{}, mc_rect{color{.5f, .5f, .5f, .5f}} };
    slider<float> sldr{ vec2{ 600,500 }, vec2{ 200, 40 }, L"example slider", &val, 0.f, 1.f, &style };

    widgets.add_widget(&sldr);
   
    widgets = get_slider_style_edit_list(&style);
    widgets.add_widget(&sldr);
    widgets.set_move_mode(true);

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
        
        //renderer.add_circle_filled({ 100, 100 }, 33.f, blue, 32);
        //renderer.add_circle({ 500, 500 }, 25.f, green, 20);
        //renderer.add_circle_filled({ 100, 200 }, 75.f, red, 32);
        //renderer.add_circle_filled({ 100, 100 }, 33.f, { 1.f, 0.f, 1.f, 1.f }, 32);
        //renderer.add_circle({ 100, 100 }, 80, red, 32);
        //renderer.add_circle({300, 100}, 75, blue, 12);
        //renderer.add_circle_filled({ 500, 100 }, 90, blue, 20);
        //
        //renderer.add_line({ 100, 200 }, { 600, 200 }, yellow);
        //renderer.add_line_multicolor({ 100, 220 }, { 600, 220 }, red, blue);
        //renderer.add_triangle({ 100, 350 }, { 150, 250 }, { 200, 350 }, green);
        //renderer.add_triangle_filled({ 250, 350 }, { 300, 250 }, { 350, 350 }, blue);
        //renderer.add_triangle_filled_multicolor({ 400, 350 }, { 450, 250 }, { 500, 350 }, blue, red, green);
        //renderer.add_rect_filled({ 100, 400 }, { 200, 200 }, white);
        //renderer.add_rect_filled_multicolor({ 350, 400 }, { 200, 200 }, red, blue, red, blue);
        //renderer.add_rect_filled_multicolor({ 600, 400 }, { 200, 200 }, red, green, yellow, blue);
        //
        //renderer.add_polyline(points, sizeof(points) / sizeof(vec2), red);
        //renderer.add_3d_wire_frame({ 500, 650 }, { 100, 100, 20 }, blue);
        //renderer.add_frame({ 650, 650 }, { 100, 75 }, 5.f, green);
        //renderer.add_frame({ 775, 650 }, { 200, 300 }, 10.f, {.5f, 0.f, 0.f, .5f});
        //
        //renderer.add_text({ 50, 800 }, L"example text", white, 100.f, text_flags::center_align);
        //btn.draw();
        
        widgets.draw_widgets();
        renderer.add_text({ 1000,25 }, { 50,20 }, L"framerate: " + std::to_wstring(last_second_frames), { 1.f }, 16.f);
        if (widgets.get_move_mode())
            renderer.add_text_with_bg({ 1000,0 }, { 50,20 }, L"Moving Enabled", { 1.f, 0.f, 0.f, 1.f }, { 0.f, 0.f, 0.f, 1.f }, 24.f);
        renderer.draw();
        frame_count++;
        auto second = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
        if (second.count() - time.count() >= 1)
        {
            last_second_frames = frame_count;
            frame_count = 0;
            time = second;
        }

        if (GetAsyncKeyState(VK_INSERT) & 0x1)
            widgets.toggle_move_mode();
        if (GetAsyncKeyState(VK_HOME) & 0x1)
            std::cout << widgets.to_string() << std::endl;
    }
    
    //std::cin.get();
    renderer.cleanup();
    // (auto p : widgets)
        //delete p;
}

int main()
{
    
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