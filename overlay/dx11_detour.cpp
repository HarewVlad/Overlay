ID3D11RenderTargetView *CreateRTV(IDXGISwapChain *swap_chain, ID3D11Device *device) {
  ID3D11RenderTargetView *result = nullptr;
  ID3D11Texture2D *back_buffer = nullptr;
  swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer)); // TODO: Add error capturing
  device->CreateRenderTargetView(back_buffer, NULL, &result); // TODO: Add error capturing

  back_buffer->Release();
  return result;
}

bool InitializeDx11(IDXGISwapChain *swap_chain) {
  DXGI_SWAP_CHAIN_DESC swap_chain_desc;
  HRESULT success = swap_chain->GetDesc(&swap_chain_desc);
  if (FAILED(success)) {
    LOG(Log_Error, "<GetDesc> failed, error = %d", success);
    return false;    
  }

  swap_chain->GetDevice(IID_PPV_ARGS(&Global_Dx11Data.m_device));

  Global_Dx11Data.m_rtv = CreateRTV(swap_chain, Global_Dx11Data.m_device);
  Global_Dx11Data.m_device->GetImmediateContext(&Global_Dx11Data.m_device_context);

  // NOTE(Vlad): Initialize window input hook first
  if (!WindowHook(swap_chain_desc.OutputWindow)) {
    LOG(Log_Error, "<WindowHook> failed");
    return false;
  }

  // NOTE(Vlad): Initialize texture to copy frames
  if (!InitializeFrame(swap_chain, Global_Dx11Data.m_device)) {
    LOG(Log_Error, "<InitializeFrame> failed");
    return false;
  }

  ImGuiInitializeWin32(swap_chain_desc.OutputWindow);
  ImGuiInitializeDx11(Global_Dx11Data.m_device, Global_Dx11Data.m_device_context);

  return true;
}

HRESULT WINAPI PresentHook(IDXGISwapChain *swap_chain, UINT sync_interval,
                           UINT flags) {
  auto present = (Dx11Data::Present)Global_Dx11Data.m_Present->m_original;

  // TODO: Hook other swap chain methods to detect reset
  if (GetState(State_Close)) {
    return present(swap_chain, sync_interval, flags);
  }

  if (!GetState(State_Initialized)) {
    if (!InitializeDx11(swap_chain)) {
      LOG(Log_Error, "<InitializeDx11> failed");

      SetState(State_Close);

      return present(swap_chain, sync_interval, flags);
    }

    SetState(State_Initialized);
  }

  // NOTE(Vlad): Capture on request
  if (GetState(State_Screenshot)) {
    if (!CaptureFrame(swap_chain, Global_Dx11Data.m_device_context)) {
      LOG(Log_Error, "<CaptureFrame> failed");
    }

    RemoveState(State_Screenshot);
  }

  // NOTE(Vlad): Draw
  ImGuiBeginDx11();

  ImGuiDraw();

  Global_Dx11Data.m_device_context->OMSetRenderTargets(1, &Global_Dx11Data.m_rtv, NULL);
  ImGuiEndDx11();

  HRESULT result = present(swap_chain, sync_interval, flags);

  return result;
}

HRESULT WINAPI ResizeBuffersHook(IDXGISwapChain *swap_chain, UINT buffer_count,
                                 UINT width, UINT height,
                                 DXGI_FORMAT new_format,
                                 UINT swap_chain_flags) {

  auto resize_buffers = (Dx11Data::ResizeBuffers)Global_Dx11Data.m_ResizeBuffers->m_original;

  if (Global_Dx11Data.m_rtv) {
    Global_Dx11Data.m_rtv->Release();
    Global_Dx11Data.m_rtv = nullptr;
  }

  HRESULT result = resize_buffers(swap_chain, buffer_count, width, height, new_format, swap_chain_flags);

  Global_Dx11Data.m_rtv = CreateRTV(swap_chain, Global_Dx11Data.m_device);

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

  Dx11Data::CreateDeviceAndSwapChain create_swap_chain_and_present =
      (Dx11Data::CreateDeviceAndSwapChain)GetProcAddress(
          GetModuleHandle("d3d11.dll"), "D3D11CreateDeviceAndSwapChain");
  if (FAILED(create_swap_chain_and_present(
          nullptr, D3D_DRIVER_TYPE_NULL, nullptr, 0, &feature_level, 1,
          D3D11_SDK_VERSION, &swap_chain_desc, &swap_chain, &device, nullptr,
          &context))) {
    LOG(Log_Error, "<create_swap_chain_and_present> failed");
    return false;
  }

  uintptr_t present_address = (*(uintptr_t **)swap_chain)[8];
  uintptr_t resize_buffers_address = (*(uintptr_t **)swap_chain)[13];

  LOG(Log_Info, "Present address -> %x", present_address);
  LOG(Log_Info, "ResizeBuffers address -> %x", resize_buffers_address);

  Global_Dx11Data.m_Present = CreateHook((PVOID *)present_address, PresentHook);
  if (!Global_Dx11Data.m_Present) {
    LOG(Log_Error, "<CreateHook> failed");
    return false;
  }

  Global_Dx11Data.m_ResizeBuffers =
      CreateHook((PVOID *)resize_buffers_address, ResizeBuffersHook);
  if (!Global_Dx11Data.m_ResizeBuffers) {
    LOG(Log_Error, "<CreateHook failed");
    return false;
  }

  if (!EnableHook(Global_Dx11Data.m_Present)) {
    LOG(Log_Error, "<EnableHook> failed");
    return false;
  }

  if (!EnableHook(Global_Dx11Data.m_ResizeBuffers)) {
    LOG(Log_Error, "<EnableHook> failed");
    return false;
  }

  context->Release();
  device->Release();
  swap_chain->Release();

  return true;
}

void Dx11Shutdown() {
  if (Global_Dx11Data.m_Present) RemoveHook(Global_Dx11Data.m_Present);
  if (Global_Dx11Data.m_ResizeBuffers) RemoveHook(Global_Dx11Data.m_ResizeBuffers);

  if (Global_Dx11Data.m_device) Global_Dx11Data.m_device->Release();
  if (Global_Dx11Data.m_rtv) Global_Dx11Data.m_rtv->Release();

  delete Global_Dx11Data.m_Present;
  delete Global_Dx11Data.m_ResizeBuffers;
}
