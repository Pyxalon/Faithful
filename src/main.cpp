#include <windows.h>
#include <d3d11.h>
#include <tchar.h>
#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"
#include "ui/Dashboard.hpp"

static ID3D11Device* device=nullptr;
static ID3D11DeviceContext* context=nullptr;
static IDXGISwapChain* swapChain=nullptr;
static ID3D11RenderTargetView* renderTarget=nullptr;
static UINT resizeWidth=0, resizeHeight=0;
static void CreateRenderTarget() {
    ID3D11Texture2D* backBuffer=nullptr;
    if(SUCCEEDED(swapChain->GetBuffer(0,IID_PPV_ARGS(&backBuffer)))) {
        device->CreateRenderTargetView(backBuffer,nullptr,&renderTarget);
        backBuffer->Release();
    }
}
static void CleanupRenderTarget(){ if(renderTarget){renderTarget->Release();renderTarget=nullptr;} }
static bool CreateDevice(HWND hwnd) {
    DXGI_SWAP_CHAIN_DESC desc{};
    desc.BufferCount=2; desc.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT; desc.OutputWindow=hwnd;
    desc.SampleDesc.Count=1; desc.Windowed=TRUE; desc.SwapEffect=DXGI_SWAP_EFFECT_DISCARD;
    D3D_FEATURE_LEVEL level{};
    const D3D_FEATURE_LEVEL levels[]={D3D_FEATURE_LEVEL_11_0,D3D_FEATURE_LEVEL_10_0};
    HRESULT hr=D3D11CreateDeviceAndSwapChain(nullptr,D3D_DRIVER_TYPE_HARDWARE,nullptr,0,
        levels,2,D3D11_SDK_VERSION,&desc,&swapChain,&device,&level,&context);
    if(FAILED(hr)) return false;
    CreateRenderTarget(); return renderTarget!=nullptr;
}
static void CleanupDevice(){ CleanupRenderTarget(); if(swapChain)swapChain->Release(); if(context)context->Release(); if(device)device->Release(); }
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND,UINT,WPARAM,LPARAM);
static LRESULT WINAPI WndProc(HWND hwnd,UINT msg,WPARAM w,LPARAM l) {
    if(ImGui::GetCurrentContext() && ImGui_ImplWin32_WndProcHandler(hwnd,msg,w,l)) return true;
    switch(msg) {
    case WM_SIZE: if(w!=SIZE_MINIMIZED){resizeWidth=LOWORD(l);resizeHeight=HIWORD(l);} return 0;
    case WM_SYSCOMMAND: if((w&0xfff0)==SC_KEYMENU)return 0; break;
    case WM_DESTROY: PostQuitMessage(0);return 0;
    }
    return DefWindowProcW(hwnd,msg,w,l);
}
int WINAPI wWinMain(HINSTANCE instance,HINSTANCE,PWSTR,int) {
    WNDCLASSEXW wc={sizeof(wc),CS_CLASSDC,WndProc,0,0,instance,nullptr,nullptr,nullptr,nullptr,L"FaithfulWindow",nullptr};
    RegisterClassExW(&wc);
    HWND hwnd=CreateWindowW(wc.lpszClassName,L"Faithful - Your Daily Walk",WS_OVERLAPPEDWINDOW,
        100,100,1180,760,nullptr,nullptr,instance,nullptr);
    if(!CreateDevice(hwnd)){CleanupDevice();DestroyWindow(hwnd);UnregisterClassW(wc.lpszClassName,instance);return 1;}
    ShowWindow(hwnd,SW_SHOWDEFAULT);UpdateWindow(hwnd);
    IMGUI_CHECKVERSION(); ImGui::CreateContext();
    ImGuiIO& io=ImGui::GetIO(); io.ConfigFlags|=ImGuiConfigFlags_NavEnableKeyboard;
    Faithful::UI::ApplyTheme();
    ImGui_ImplWin32_Init(hwnd);ImGui_ImplDX11_Init(device,context);
    bool done=false;
    while(!done) {
        MSG msg;
        while(PeekMessageW(&msg,nullptr,0,0,PM_REMOVE)) {
            TranslateMessage(&msg);DispatchMessageW(&msg);
            if(msg.message==WM_QUIT)done=true;
        }
        if(done)break;
        if(resizeWidth&&resizeHeight){CleanupRenderTarget();swapChain->ResizeBuffers(0,resizeWidth,resizeHeight,DXGI_FORMAT_UNKNOWN,0);resizeWidth=resizeHeight=0;CreateRenderTarget();}
        ImGui_ImplDX11_NewFrame();ImGui_ImplWin32_NewFrame();ImGui::NewFrame();
        Faithful::UI::RenderDashboard();
        ImGui::Render();
        const float clear[]={.055f,.075f,.105f,1};
        context->OMSetRenderTargets(1,&renderTarget,nullptr);
        context->ClearRenderTargetView(renderTarget,clear);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        swapChain->Present(1,0);
    }
    ImGui_ImplDX11_Shutdown();ImGui_ImplWin32_Shutdown();ImGui::DestroyContext();
    CleanupDevice();DestroyWindow(hwnd);UnregisterClassW(wc.lpszClassName,instance);
    return 0;
}
