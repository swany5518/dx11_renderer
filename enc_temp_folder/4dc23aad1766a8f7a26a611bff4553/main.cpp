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

inline std::vector<widget*> widgets;

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    vec2 pos;
    vec2 pos2;
    switch (message)
    {
    case WM_LBUTTONDOWN:
        pos = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        std::cout << "LEFT MOUSE CLICKED [ X: " << pos.x << " Y: " << pos.y << " ]" << std::endl;

        for (auto& w : widgets)
            if (w->contains(pos) && w->get_type() == widget_type::checkbox)
            {
                std::cout << "calling virtual lbutton down" << std::endl;
                w->on_lbutton_down();
            }
                
        break;

    case WM_LBUTTONUP:
        pos2 = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        std::cout << "LEFT MOUSE RELEASED [ X: " << pos2.x << " Y: " << pos2.y << " ]" << std::endl;

        for (auto& w : widgets)
            if (w->contains(pos2) && w->has_focus && w->get_type() == widget_type::checkbox)
            {
                std::cout << "calling virtual lbutton up" << std::endl;
                w->on_lbutton_up();
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
    freopen("conin$", "r", stdin);
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

    hWnd = CreateWindowEx(NULL, L"WindowClass", L"prism's first dx11 program", WS_OVERLAPPEDWINDOW, 300, 300, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);

    renderer renderer{};
    renderer.initialize(hWnd);
    widget::set_renderer(&renderer);
    bool val = 1;
    checkbox box{ {100, 100}, {99, 99}, "lol",  &val };
    widgets.push_back(&box);

    color red{ 1.f, 0.f, 0.f, .5f };
    color green{ 0.f, 1.f, 0.f, 1.f };
    color blue{ 0.f, 0.f, 1.f, 0.5f };
    color black{ 0.f, 0.f, 0.f, 1.f };
    color white{ 1.f, 1.f, 1.f, 1.f };
    color yellow{ 1.f, 1.f, 0.f, 1.f };

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
        static vec2 top_left{ 100.f, 100.f };
        static vec2 size{ 100.f, 100.f };
        static vec2 pos{ 0.f, 0.f };
        //renderer.add_rect_filled_multicolor(top_left, size, red, red, yellow, yellow);
        //renderer.add_triangle_filled_multicolor({ 300.f, 300.f }, { 450.f, 100.f }, top_left, yellow, blue, green);
        //renderer.add_text({ 0.f, 0.f }, L"lmaoooooo", blue, 10.f);
        // renderer.add_text({ 200.f, 200.f }, L"added a text renderer", blue, 100.f);
        for (auto& w : widgets)
            w->draw();

        //renderer.add_3d_wire_frame(top_left, { 200.f, 200.f, 50.f }, yellow);

        //renderer.add_circle_filled({ 500.f, 500.f }, 500.f, red, 32);
        //renderer.add_line(top_left, { 500.f, 500.f }, blue);
        //renderer.add_line_multicolor({ -.5f, .5f }, blue, { .5f, -.5f }, green);
        renderer.draw();
    }

    renderer.cleanup();
}

int main()
{
    std::string str{};
    str.reserve(129);
    std::cout << str.size() << std::endl;
    std::cout << str.capacity() << std::endl;


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