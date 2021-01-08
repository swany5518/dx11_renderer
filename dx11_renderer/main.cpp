#include <iostream>
#include <windowsx.h>
#include <vector>
#include <d3dcompiler.h>

#include "renderer.h"
#include "widgets.h"
#include "shaders.hpp"

#pragma comment(lib, "d3dcompiler.lib")

// define the screen resolution
#define SCREEN_WIDTH  1000
#define SCREEN_HEIGHT 1000
inline static float test = 5.f;
inline static float test2 = 5.f;

inline static slider<float> slide{ vec2{ 100.f, 100.f }, { 200, 20 }, L"slider",  &test, 0.f, 20.f };
inline static slider<float> slide1{ vec2{ 100.f, 130.f }, { 300, 20 }, L"slider", &test2, 0.f, 20.f };


inline std::vector<widget*> widgets = {
    &slide,
    &slide1
};

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    vec2 pos;
    switch (message)
    {
    case WM_LBUTTONDOWN:
        std::cout << "LBUTTON DOWN" << std::endl;
        pos = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        for (auto w : widgets)
            if (w->contains(pos))
            {
                std::cout << "setting clicking to true from wndproc" << std::endl;
                w->mouse_info.clicking = true;
            }
        break;

    case WM_LBUTTONUP:
        std::cout << "LBUTTON UP" << std::endl;
        for (auto w : widgets)
        {
            if (w->mouse_info.clicking && w->contains(pos))
                w->mouse_info.clicked = true;

            w->mouse_info.clicking = false;
        }
        break;
    case WM_MOUSEMOVE:
        pos = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        std::cout << "MOUSEMOVE X: " << pos.x << " Y: " << pos.y << std::endl;
    
        for (auto w : widgets)
        {
            if (w->mouse_info.clicking)
            {
                std::cout << "calling on_drag from wndproc" << std::endl;
                w->on_drag(pos);
            }
        }
        break;
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    } break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
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

    color red{ 1.f, 0.f, 0.f, 1.f };
    color green{ 0.f, 1.f, 0.f, 1.f };
    color blue{ 0.f, 0.f, 1.f, 0.5f };
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
        
        for (auto w : widgets)
            w->draw();

        //renderer.add_circle({ 100.f, 100.f }, 10.f, blue, 20);
        renderer.draw();
    }

    renderer.cleanup();
    for (auto p : widgets)
        delete p;
}

int main()
{
    mouse_state ms{};

    std::cout << ms.state+0 << std::endl;


    return 0;
    ID3DBlob* blob = nullptr;
    D3DCompile(shader::shader, sizeof(shader::shader), nullptr, nullptr, nullptr, "PS", "ps_4_0", 0, 0, &blob, nullptr);
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

    blob->Release();
}