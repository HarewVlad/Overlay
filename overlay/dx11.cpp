HRESULT WINAPI PresentHook(IDXGISwapChain *swap_chain, UINT sync_interval,
                           UINT flags) {
  auto present = (Dx11::Present)Global_Dx11.m_Present->m_original;

  static bool is_gui_initialized = false;
  if (!is_gui_initialized) {
    DXGI_SWAP_CHAIN_DESC swap_chain_desc;
    swap_chain->GetDesc(&swap_chain_desc);

    ID3D11Device *device = nullptr;
    swap_chain->GetDevice(IID_PPV_ARGS(&device));

    ID3D11Texture2D *back_buffer = nullptr;
    swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));

    ID3D11RenderTargetView *rtv = nullptr;
    device->CreateRenderTargetView(back_buffer, NULL, &rtv);
    back_buffer->Release();

    ID3D11DeviceContext *device_context = nullptr;
    device->GetImmediateContext(&device_context);

    ImGuiInitializeWin32(swap_chain_desc.OutputWindow);
    ImGuiInitializeDx11(device, device_context);

    is_gui_initialized = true;
  }

  ImGuiBeginDx11();

  ImGuiDraw();

  ImGuiEndDx11();

  HRESULT result = present(swap_chain, sync_interval, flags);

  return result;
}

HRESULT WINAPI ResizeBuffersHook(IDXGISwapChain *swap_chain, UINT buffer_count,
                                 UINT width, UINT height,
                                 DXGI_FORMAT new_format,
                                 UINT swap_chain_flags) {

  auto resize_buffers = (Dx11::ResizeBuffers)Global_Dx11.m_ResizeBuffers->m_original;

  HRESULT result = resize_buffers(swap_chain, buffer_count, width, height, new_format, swap_chain_flags);

  return result;
}

bool Dx11Hook(HWND window) {
  IDXGISwapChain *swap_chain = nullptr;
  ID3D11Device *device = nullptr;
  ID3D11DeviceContext *context = nullptr;

  D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;
  DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
  swap_chain_desc.BufferCount = 1;
  swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swap_chain_desc.OutputWindow = window;
  swap_chain_desc.SampleDesc.Count = 1;
  swap_chain_desc.Windowed = TRUE;
  swap_chain_desc.BufferDesc.ScanlineOrdering =
      DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
  swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  Dx11::CreateDeviceAndSwapChain create_swap_chain_and_present =
      (Dx11::CreateDeviceAndSwapChain)GetProcAddress(
          GetModuleHandle("d3d11.dll"), "D3D11CreateDeviceAndSwapChain");
  if (FAILED(create_swap_chain_and_present(
          nullptr, D3D_DRIVER_TYPE_NULL, nullptr, 0, &feature_level, 1,
          D3D11_SDK_VERSION, &swap_chain_desc, &swap_chain, &device, nullptr,
          &context))) {
    Log("ERROR", "<create_swap_chain_and_present> failed");
    return false;
  }

  uintptr_t present_address = (*(uintptr_t **)swap_chain)[8];
  uintptr_t resize_buffers_address = (*(uintptr_t **)swap_chain)[13];

  Log("INFO", "Present address -> %x", present_address);
  Log("INFO", "ResizeBuffers address -> %x", resize_buffers_address);

  Global_Dx11.m_Present = CreateHook((PVOID *)present_address, PresentHook);
  if (!Global_Dx11.m_Present) {
    Log("ERROR", "<CreateHook> failed");
    return false;
  }

  Global_Dx11.m_ResizeBuffers =
      CreateHook((PVOID *)resize_buffers_address, ResizeBuffersHook);
  if (!Global_Dx11.m_ResizeBuffers) {
    Log("ERROR", "<CreateHook failed");
    return false;
  }

  context->Release();
  device->Release();
  swap_chain->Release();

  return true;
}

void Dx11Shutdown() {
  if (Global_Dx11.m_Present) RemoveHook(Global_Dx11.m_Present);
  if (Global_Dx11.m_ResizeBuffers) RemoveHook(Global_Dx11.m_ResizeBuffers);

  if (Global_Dx11.m_device) Global_Dx11.m_device->Release();
  if (Global_Dx11.m_rtv) Global_Dx11.m_rtv->Release();
}
