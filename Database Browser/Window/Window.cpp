#include "Window.h"
#include "../App/SQLiteDatabaseStore.h"
#include "Menu.h"

namespace Window
{
	ID3D11Device* pD3DDevice = nullptr;
	ID3D11DeviceContext* pD3DDeviceContext = nullptr;
    IDXGISwapChain* pSwapChain = nullptr;
	ID3D11RenderTargetView* pMainRenderTargetView = nullptr;
}

bool Window::CreateDevice(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &pSwapChain, &pD3DDevice, &featureLevel, &pD3DDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_WARP, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &pSwapChain, &pD3DDevice, &featureLevel, &pD3DDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void Window::CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    pD3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &pMainRenderTargetView);
    pBackBuffer->Release();
}

void Window::DestroyRenderTarget()
{
    if (pMainRenderTargetView) pMainRenderTargetView->Release(); pMainRenderTargetView = nullptr;
}

void Window::DestroyDevice()
{
    if (pSwapChain) pSwapChain->Release(); pSwapChain = nullptr;
    if (pD3DDeviceContext) pD3DDeviceContext->Release(); pD3DDeviceContext = nullptr;
    if (pD3DDevice) pD3DDevice->Release(); pD3DDevice = nullptr;
}

HWND Window::CreateAppWindow()
{
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"ImGui Example", NULL };
    RegisterClassExW(&wc);
    HWND hWnd = ::CreateWindowW(wc.lpszClassName, L"Database Browser", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, wc.hInstance, NULL);

    if (!CreateDevice(hWnd))
    {
        DestroyDevice();
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return NULL;
    }

    ShowWindow(hWnd, SW_SHOWDEFAULT);
    UpdateWindow(hWnd);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(pD3DDevice, pD3DDeviceContext);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    bool done = false;
    while (!done)
    {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowPos(viewport->Pos);

        // Visuals rendering
        Menu::DrawMenu();

        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        pD3DDeviceContext->OMSetRenderTargets(1, &pMainRenderTargetView, NULL);
        pD3DDeviceContext->ClearRenderTargetView(pMainRenderTargetView, clear_color_with_alpha);

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        pSwapChain->Present(1, 0);
    };

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    DestroyDevice();
    DestroyWindow(hWnd);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return hWnd;
}

LRESULT WINAPI Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (pD3DDevice != nullptr && wParam != SIZE_MINIMIZED)
        {
            DestroyRenderTarget();
            pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}