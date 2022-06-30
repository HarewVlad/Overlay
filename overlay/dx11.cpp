ID3D11RenderTargetView *CreateRTV(IDXGISwapChain *swap_chain, ID3D11Device *device) {
  ID3D11RenderTargetView *result = nullptr;
  ID3D11Texture2D *back_buffer = nullptr;
  swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer)); // TODO: Add error capturing
  device->CreateRenderTargetView(back_buffer, NULL, &result); // TODO: Add error capturing

  back_buffer->Release();
  return result;
}

struct TextureInfo {
  unsigned int m_width;
  unsigned int m_height;
  DXGI_FORMAT m_format;
  bool m_multisampled;
};

bool GetTextureInfo(IDXGISwapChain *swap_chain, TextureInfo *texture_info) {
  DXGI_SWAP_CHAIN_DESC swap_chain_desc;
  HRESULT result = swap_chain->GetDesc(&swap_chain_desc);
  if (FAILED(result)) {
    Log(Log_Error, "<GetDesc> failed, error = %d", result);
    return false;
  }

  texture_info->m_format = swap_chain_desc.BufferDesc.Format;
  texture_info->m_multisampled = swap_chain_desc.SampleDesc.Count > 1;
  texture_info->m_width = swap_chain_desc.BufferDesc.Width;
  texture_info->m_height = swap_chain_desc.BufferDesc.Height;

  return true;
}

ID3D11Texture2D *CreateSharedTexture(ID3D11Device *device, TextureInfo *texture_info) {
  ID3D11Texture2D *texture = nullptr;

  D3D11_TEXTURE2D_DESC texture_desc = {};
  texture_desc.Width = texture_info->m_width;
  texture_desc.Height = texture_info->m_height;
  texture_desc.Format = texture_info->m_format;
  texture_desc.MipLevels = 1;
  texture_desc.ArraySize = 1;
  texture_desc.SampleDesc.Count = 1;
  texture_desc.Usage = D3D11_USAGE_STAGING;
  texture_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

  HRESULT result = device->CreateTexture2D(&texture_desc, nullptr, &texture);
  if (FAILED(result)) {
    Log(Log_Error, "<CreateTexture2D> failed, error = %d", result);
    return nullptr;
  }

  return texture;
}

void CaptureFrame(IDXGISwapChain *swap_chain, ID3D11DeviceContext *device_context, TextureInfo *texture_info, ID3D11Texture2D *texture) {
  ID3D11Texture2D *back_buffer = nullptr;
  swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer)); // TODO: Add error capturing

  if (!texture_info->m_multisampled) {
    device_context->CopyResource(texture, back_buffer);
  } else {
    device_context->ResolveSubresource(texture, 0, back_buffer, 0, texture_info->m_format);
  }

  back_buffer->Release();

  // TODO: Save to file?
  // D3DX11SaveTextureToFile();
}

HRESULT WINAPI PresentHook(IDXGISwapChain *swap_chain, UINT sync_interval,
                           UINT flags) {
  auto present = (Dx11::Present)Global_Dx11.m_Present->m_original;

  // TODO: Hook other swap chain methods to detect reset
  if (Global_ShouldClose) {
    return present(swap_chain, sync_interval, flags);
  }

  if (!Global_IsInitialized) {
    DXGI_SWAP_CHAIN_DESC swap_chain_desc;
    if (FAILED(swap_chain->GetDesc(&swap_chain_desc))) {
      Log(Log_Error, "<GetDesc> failed");

      Global_ShouldClose = true;
      return present(swap_chain, sync_interval, flags);
    }

    swap_chain->GetDevice(IID_PPV_ARGS(&Global_Dx11.m_device));

    Global_Dx11.m_rtv = CreateRTV(swap_chain, Global_Dx11.m_device);
    Global_Dx11.m_device->GetImmediateContext(&Global_Dx11.m_device_context);

    // NOTE(Vlad): Initialize window input hook first
    if (!WindowHook(swap_chain_desc.OutputWindow)) {
      Log(Log_Error, "<WindowHook> failed");

      Global_ShouldClose = true;
      return present(swap_chain, sync_interval, flags);
    }

    // NOTE(Vlad): Initialize texture to copy frames
    TextureInfo texture_info = {};
    if (!GetTextureInfo(swap_chain, &texture_info)) {
      Log(Log_Error, "<GetTextureInfo> failed");

      Global_ShouldClose = true;
      return present(swap_chain, sync_interval, flags); 
    }

    Global_Dx11.m_texture = CreateSharedTexture(Global_Dx11.m_device, &texture_info);

    CaptureFrame(swap_chain, Global_Dx11.m_device_context, &texture_info, Global_Dx11.m_texture);

    ImGuiInitializeWin32(swap_chain_desc.OutputWindow);
    ImGuiInitializeDx11(Global_Dx11.m_device, Global_Dx11.m_device_context);

    Global_IsInitialized = true;
  }

  if (Global_IsInitialized) {
    ImGuiBeginDx11();

    ImGuiDraw();

    Global_Dx11.m_device_context->OMSetRenderTargets(1, &Global_Dx11.m_rtv, NULL);
    ImGuiEndDx11();
  }

  HRESULT result = present(swap_chain, sync_interval, flags);

  return result;
}

HRESULT WINAPI ResizeBuffersHook(IDXGISwapChain *swap_chain, UINT buffer_count,
                                 UINT width, UINT height,
                                 DXGI_FORMAT new_format,
                                 UINT swap_chain_flags) {

  auto resize_buffers = (Dx11::ResizeBuffers)Global_Dx11.m_ResizeBuffers->m_original;

  if (Global_Dx11.m_rtv) {
    Global_Dx11.m_rtv->Release();
    Global_Dx11.m_rtv = nullptr;
  }

  HRESULT result = resize_buffers(swap_chain, buffer_count, width, height, new_format, swap_chain_flags);

  Global_Dx11.m_rtv = CreateRTV(swap_chain, Global_Dx11.m_device);

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
    Log(Log_Error, "<create_swap_chain_and_present> failed");
    return false;
  }

  uintptr_t present_address = (*(uintptr_t **)swap_chain)[8];
  uintptr_t resize_buffers_address = (*(uintptr_t **)swap_chain)[13];

  Log(Log_Info, "Present address -> %x", present_address);
  Log(Log_Info, "ResizeBuffers address -> %x", resize_buffers_address);

  Global_Dx11.m_Present = CreateHook((PVOID *)present_address, PresentHook);
  if (!Global_Dx11.m_Present) {
    Log(Log_Error, "<CreateHook> failed");
    return false;
  }

  Global_Dx11.m_ResizeBuffers =
      CreateHook((PVOID *)resize_buffers_address, ResizeBuffersHook);
  if (!Global_Dx11.m_ResizeBuffers) {
    Log(Log_Error, "<CreateHook failed");
    return false;
  }

  if (!EnableHook(Global_Dx11.m_Present)) {
    Log(Log_Error, "<EnableHook> failed");
    return false;
  }

  if (!EnableHook(Global_Dx11.m_ResizeBuffers)) {
    Log(Log_Error, "<EnableHook> failed");
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
