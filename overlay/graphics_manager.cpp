GraphicsManager *GraphicsManager::m_instance = nullptr;

bool GraphicsManager::CreateRTV(IDXGISwapChain *swap_chain) {
  ID3D11Texture2D *back_buffer = nullptr;
  HRESULT success = swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer)); // TODO: Add error capturing
  if (FAILED(success)) {
    Log(Log_Error, "<GetBuffer> failed, error = %d", success);
    return false;
  }

  success = m_device->CreateRenderTargetView(back_buffer, NULL, &m_rtv); // TODO: Add error capturing
  if (FAILED(success)) {
    Log(Log_Error, "<CreateRenderTargetView> failed, error = %d", success);
    return false;
  }

  back_buffer->Release();

  return true;
}

bool GraphicsManager::Initialize(IDXGISwapChain *swap_chain) {
  DXGI_SWAP_CHAIN_DESC swap_chain_desc;
  HRESULT success = swap_chain->GetDesc(&swap_chain_desc);
  if (FAILED(success)) {
    Log(Log_Error, "<GetDesc> failed, error = %d", success);
    return false;    
  }

  swap_chain->GetDevice(IID_PPV_ARGS(&m_device));
  CreateRTV(swap_chain);
  m_device->GetImmediateContext(&m_device_context);

  // NOTE(Vlad): Initialize copy texture
  D3D11_TEXTURE2D_DESC texture_desc = {};
  texture_desc.Width = swap_chain_desc.BufferDesc.Width;
  texture_desc.Height = swap_chain_desc.BufferDesc.Height;
  texture_desc.Format = swap_chain_desc.BufferDesc.Format;
  texture_desc.MipLevels = 1;
  texture_desc.ArraySize = 1;
  texture_desc.SampleDesc.Count = 1;
  texture_desc.Usage = D3D11_USAGE_STAGING;
  texture_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

  success = m_device->CreateTexture2D(&texture_desc, nullptr, &m_copy_texture);
  if (FAILED(success)) {
    Log(Log_Error, "<CreateTexture2D> failed, error = %d", success);
    return false;
  }

  m_multisampled = swap_chain_desc.SampleDesc.Count > 1;
  m_format = swap_chain_desc.BufferDesc.Format;

  m_window_manager.Initialize(swap_chain_desc.OutputWindow);

  if (!m_window_manager.HookFunctions()) {
    Log(Log_Error, "Failed to hook window functions");
    return false;
  }

  // TODO: Refactor this
  ImGuiInitializeWin32(swap_chain_desc.OutputWindow);
  ImGuiInitializeGraphics(m_device, m_device_context);

  return true;
}

static HRESULT WINAPI PresentStatic(IDXGISwapChain *swap_chain, UINT sync_interval,
                           UINT flags) {
  return GraphicsManager::m_instance->PresentHook(swap_chain, sync_interval, flags);
}

HRESULT WINAPI GraphicsManager::PresentHook(IDXGISwapChain *swap_chain, UINT sync_interval,
                           UINT flags) {
  auto present = (Present)m_Present.m_original;

  // TODO: Hook other swap chain methods to detect reset
  if (GetState(State_Close)) {
    return present(swap_chain, sync_interval, flags);
  }
  
  if (!GetState(State_Initialized)) {
    if (!Initialize(swap_chain)) {
      Log(Log_Error, "Failed to initialize graphics functions");

      SetState(State_Close);

      return present(swap_chain, sync_interval, flags);
    }

    SetState(State_Initialized);
  }
  
  if (GetState(State_Screenshot) || GetState(State_Record)) {
    ID3D11Texture2D *back_buffer = nullptr;
    HRESULT success = swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
    if (FAILED(success)) {
      Log(Log_Error, "<GetBuffer> failed, error = %d", success);
    } else {
      if (GetState(State_Screenshot)) {
        if (!SaveScreenshot(swap_chain, m_device, m_device_context)) {
          Log(Log_Error, "Failed to save screenshot");
        }

        RemoveState(State_Screenshot);
      }

      if (GetState(State_Record)) {
        // NOTE(Vlad): Capture frame
        // if (!Global_Dx11Data.m_multisampled) {
        //   Global_Dx11Data.m_device_context->CopyResource(Global_Dx11Data.m_copy_texture, back_buffer);
        // } else {
        //   Global_Dx11Data.m_device_context->ResolveSubresource(Global_Dx11Data.m_copy_texture, 0, back_buffer, 0, Global_Dx11Data.m_format);
        // }
      }
    }
  }

  // NOTE(Vlad): Draw overlay
  ImGuiBegin();
  ImGuiDraw();
  m_device_context->OMSetRenderTargets(1, &m_rtv, NULL);
  ImGuiEnd();

  HRESULT result = present(swap_chain, sync_interval, flags);

  return result;
}

static HRESULT WINAPI ResizeBuffersStatic(IDXGISwapChain *swap_chain, UINT buffer_count,
                                 UINT width, UINT height,
                                 DXGI_FORMAT new_format,
                                 UINT swap_chain_flags) {
  return GraphicsManager::m_instance->ResizeBuffersHook(swap_chain, buffer_count, width, height, new_format, swap_chain_flags);
}

HRESULT WINAPI GraphicsManager::ResizeBuffersHook(IDXGISwapChain *swap_chain, UINT buffer_count,
                                 UINT width, UINT height,
                                 DXGI_FORMAT new_format,
                                 UINT swap_chain_flags) {

  auto resize_buffers = (GraphicsManager::ResizeBuffers)m_ResizeBuffers.m_original;

  if (m_rtv) {
    m_rtv->Release();
    m_rtv = nullptr;
  }

  HRESULT result = resize_buffers(swap_chain, buffer_count, width, height, new_format, swap_chain_flags);

  CreateRTV(swap_chain);

  return result;
}

bool GraphicsManager::HookFunctions(HWND window) {
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

  CreateDeviceAndSwapChain create_swap_chain_and_present =
      (CreateDeviceAndSwapChain)GetProcAddress(
          GetModuleHandle("d3d11.dll"), "D3D11CreateDeviceAndSwapChain");
  HRESULT success = create_swap_chain_and_present(
          nullptr, D3D_DRIVER_TYPE_NULL, nullptr, 0, &feature_level, 1,
          D3D11_SDK_VERSION, &swap_chain_desc, &swap_chain, &device, nullptr,
          &context);
  if (FAILED(success)) {
    Log(Log_Error, "<create_swap_chain_and_present> failed, error = %d", success);
    return false;
  }

  uintptr_t present_address = (*(uintptr_t **)swap_chain)[8];
  uintptr_t resize_buffers_address = (*(uintptr_t **)swap_chain)[13];

  Log(Log_Info, "Present address -> %x", present_address);
  Log(Log_Info, "ResizeBuffers address -> %x", resize_buffers_address);

  m_Present.Initialize((PVOID *)present_address, PresentStatic);
  m_ResizeBuffers.Initialize((PVOID *)resize_buffers_address, ResizeBuffersStatic);

  if (!m_Present.Enable()) {
    Log(Log_Error, "Failed to enable Present hook");
    return false;
  }

  if (!m_ResizeBuffers.Enable()) {
    Log(Log_Error, "Failed to enable ResizeBuffers hook");
    return false;
  }

  context->Release();
  device->Release();
  swap_chain->Release();

  return true;
}

void GraphicsManager::UnhookFunctions() {
  m_Present.Disable();
  m_ResizeBuffers.Disable();
  m_window_manager.UnhookFunctions();
  
  if (m_device) m_device->Release();
  if (m_rtv) m_rtv->Release();
}