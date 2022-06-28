// Dear ImGui: standalone example application for DirectX 11
// If you are new to Dear ImGui, read documentation from the docs/ folder + read
// the top of imgui.cpp. Read online:
// https://github.com/ocornut/imgui/tree/master/docs

// #include "imgui.h"
// #include "imgui_impl_win32.h"
// #include "imgui_impl_dx11.h"
// #include <d3d11.h>
// #include <tchar.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxguid.lib")

namespace Test {
namespace DX11 {
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void Initialize() {
  WNDCLASSEX wc = {sizeof(WNDCLASSEX),    CS_CLASSDC, WndProc, 0L,   0L,
                   GetModuleHandle(NULL), NULL,       NULL,    NULL, NULL,
                   "Directx11",           NULL};
  RegisterClassEx(&wc);

  HWND hWnd =
      CreateWindowA(wc.lpszClassName, "Directx11", WS_OVERLAPPEDWINDOW, 100,
                    100, 1165, 876, NULL, NULL, wc.hInstance, NULL);

  if (!CreateDeviceD3D(hWnd)) {
    CleanupDeviceD3D();
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    Log("ERROR", "Failed to initialize Directx11");
    assert(0);
  }

  ShowWindow(hWnd, SW_SHOWDEFAULT);
  UpdateWindow(hWnd);

  // Setup Dear ImGui context
  // IMGUI_CHECKVERSION();
  // ImGui::CreateContext();
  // ImGuiIO& io = ImGui::GetIO(); (void)io;
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable
  // Keyboard Controls io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; //
  // Enable Gamepad Controls

  // Setup Platform/Renderer backends
  // ImGui_ImplWin32_Init(hWnd);
  // ImGui_ImplDX11_Init(Global_Device, Global_DeviceContext);

  bool show_demo_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  bool done = false;
  while (!done) {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);

      if (GetAsyncKeyState('W') & 0x8000) {
        ShutdownOverlay();
      }
      else if (GetAsyncKeyState('S') & 0x8000) {
        InitializeOverlay(GetModuleHandle(NULL));
      }

      if (msg.message == WM_QUIT)
        done = true;
    }

    if (done)
      break;

    // Start the Dear ImGui frame
    // ImGui_ImplDX11_NewFrame();
    // ImGui_ImplWin32_NewFrame();
    // ImGui::NewFrame();

    // 1. Show the big demo window (Most of the sample code is in
    // ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear
    // ImGui!).
    // if (show_demo_window)
    // ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair
    // to created a named window.
    //{
    //  static float f = 0.0f;
    //  static int counter = 0;

    //  ImGui::Begin("Hello, world!");                          // Create a
    //  window called "Hello, world!" and append into it.

    //  ImGui::Text("This is some useful text.");               // Display some
    //  text (you can use a format strings too) ImGui::Checkbox("Demo Window",
    //  &show_demo_window);      // Edit bools storing our window open/close
    //  state ImGui::Checkbox("Another Window", &show_another_window);

    //  ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float
    //  using a slider from 0.0f to 1.0f ImGui::ColorEdit3("clear color",
    //  (float*)&clear_color); // Edit 3 floats representing a color

    //  if (ImGui::Button("Button"))                            // Buttons
    //  return true when clicked (most widgets return true when
    //  edited/activated)
    //    counter++;
    //  ImGui::SameLine();
    //  ImGui::Text("counter = %d", counter);

    //  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f /
    //  ImGui::GetIO().Framerate, ImGui::GetIO().Framerate); ImGui::End();
    //}

    // 3. Show another simple window.
    // if (show_another_window)
    //{
    //  ImGui::Begin("Another Window", &show_another_window);   // Pass a
    //  pointer to our bool variable (the window will have a closing button that
    //  will clear the bool when clicked) ImGui::Text("Hello from another
    //  window!"); if (ImGui::Button("Close Me"))
    //    show_another_window = false;
    //  ImGui::End();
    //}

    // Rendering
    // ImGui::Render();
    const float clear_color_with_alpha[4] = {
        clear_color.x * clear_color.w, clear_color.y * clear_color.w,
        clear_color.z * clear_color.w, clear_color.w};
    Global_DeviceContext->OMSetRenderTargets(1, &Global_Rtv, NULL);
    Global_DeviceContext->ClearRenderTargetView(Global_Rtv,
                                                clear_color_with_alpha);
    // ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    Global_SwapChain->Present(1, 0); // Present with vsync
    // g_pSwapChain->Present(0, 0); // Present without vsync
  }

  // Cleanup
  /*ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();*/

  CleanupDeviceD3D();
  DestroyWindow(hWnd);
  UnregisterClass(wc.lpszClassName, wc.hInstance);
}

bool CreateDeviceD3D(HWND hWnd) {
  DXGI_SWAP_CHAIN_DESC sd = {};
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

  UINT create_device_flags = 0;
  // create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
  D3D_FEATURE_LEVEL feauture_level;
  const D3D_FEATURE_LEVEL feauture_level_array[2] = {
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_0,
  };
  if (D3D11CreateDeviceAndSwapChain(
          NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, create_device_flags,
          feauture_level_array, 2, D3D11_SDK_VERSION, &sd, &Global_SwapChain,
          &Global_Device, &feauture_level, &Global_DeviceContext) != S_OK)
    return false;

  CreateRenderTarget();
  return true;
}

void CleanupDeviceD3D() {
  CleanupRenderTarget();
  if (Global_SwapChain) {
    Global_SwapChain->Release();
    Global_SwapChain = NULL;
  }
  if (Global_DeviceContext) {
    Global_DeviceContext->Release();
    Global_DeviceContext = NULL;
  }
  if (Global_Device) {
    Global_Device->Release();
    Global_Device = NULL;
  }
}

void CreateRenderTarget() {
  ID3D11Texture2D *back_buffer;
  Global_SwapChain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
  Global_Device->CreateRenderTargetView(back_buffer, NULL,
                                        &Global_Rtv);
  back_buffer->Release();
}

void CleanupRenderTarget() {
  if (Global_Rtv) {
    Global_Rtv->Release();
    Global_Rtv = NULL;
  }
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    return true;

  switch (msg) {
  case WM_SIZE:
    if (Global_Device != NULL && wParam != SIZE_MINIMIZED) {
      CleanupRenderTarget();
      Global_SwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam),
                                      (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN,
                                      0);
      CreateRenderTarget();
    }
    return 0;
  case WM_SYSCOMMAND:
    if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
      return 0;
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  }
  return DefWindowProc(hWnd, msg, wParam, lParam);
}
} // namespace Directx11
} // namespace Test
