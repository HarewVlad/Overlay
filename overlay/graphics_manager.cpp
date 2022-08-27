GraphicsManager *GraphicsManager::m_instance = nullptr;

bool GraphicsManager::CreateRenderTargetView(IDXGISwapChain *swap_chain) {
  ID3D11Texture2D *back_buffer = nullptr;
  HRESULT success = swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
  if (FAILED(success)) {
    Log(Log_Error, "<GetBuffer> failed, error = %x", success);
    return false;
  }

  success = m_device->CreateRenderTargetView(back_buffer, NULL, &m_rtv);
  if (FAILED(success)) {
    Log(Log_Error, "<CreateRenderTargetView> failed, error = %x", success);
    return false;
  }

  back_buffer->Release();

  return true;
}

bool GraphicsManager::Initialize(IDXGISwapChain *swap_chain) {
  HRESULT success = swap_chain->GetDesc(&m_swap_chain_desc);
  if (FAILED(success)) {
    Log(Log_Error, "<GetDesc> failed, error = %x", success);
    return false;    
  }

  swap_chain->GetDevice(IID_PPV_ARGS(&m_device));
  if (!CreateRenderTargetView(swap_chain)) {
    Log(Log_Error, "<CreateRenderTargetView> failed");
    return false;
  }
  m_device->GetImmediateContext(&m_device_context);

  // NOTE(Vlad): Initialize copy texture
  D3D11_TEXTURE2D_DESC texture_desc = {};
  texture_desc.Width = m_swap_chain_desc.BufferDesc.Width;
  texture_desc.Height = m_swap_chain_desc.BufferDesc.Height;
  texture_desc.Format = m_swap_chain_desc.BufferDesc.Format;
  texture_desc.MipLevels = 1;
  texture_desc.ArraySize = 1;
  texture_desc.SampleDesc.Count = 1;
  texture_desc.Usage = D3D11_USAGE_STAGING;
  texture_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

  success = m_device->CreateTexture2D(&texture_desc, nullptr, &m_copy_texture);
  if (FAILED(success)) {
    Log(Log_Error, "<CreateTexture2D> failed, error = %x", success);
    return false;
  }

  m_window_manager.Initialize(m_swap_chain_desc.OutputWindow);

  if (!m_window_manager.HookFunctions()) {
    Log(Log_Error, "Failed to hook window functions");
    return false;
  }

  m_video_manager.Initialize();

  ImGuiInitializeWin32(m_swap_chain_desc.OutputWindow);
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

  if (GetState(State_StartRecording)) {
    m_video_manager.StartRecording(m_swap_chain_desc.BufferDesc.Width, m_swap_chain_desc.BufferDesc.Height);

    RemoveState(State_StartRecording);
    SetState(State_Recording);
  }

  if (GetState(State_EndRecording)) {
    m_video_manager.StopRecording();

    RemoveState(State_Recording);
    RemoveState(State_EndRecording);
  }
  
  // TODO: Instread of states, use job system or something like this
  if (GetState(State_Screenshot) || GetState(State_Recording)) {
    ID3D11Texture2D *back_buffer = nullptr;
    HRESULT success = swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
    if (FAILED(success)) {
      Log(Log_Error, "<GetBuffer> failed, error = %x", success);
    } else {
      if (m_swap_chain_desc.SampleDesc.Count > 1) {
        m_device_context->ResolveSubresource(m_copy_texture, 0, back_buffer, 0, m_swap_chain_desc.BufferDesc.Format);
      } else {
        m_device_context->CopyResource(m_copy_texture, back_buffer);
      }

      D3D11_MAPPED_SUBRESOURCE mapped_subresource;
      success = m_device_context->Map(m_copy_texture, 0, D3D11_MAP_READ, 0, &mapped_subresource);
      if (FAILED(success)) {
        Log(Log_Error, "<Map> failed, error = %x", success);
        return present(swap_chain, sync_interval, flags);
      }

      if (GetState(State_Screenshot)) {
        int width = m_swap_chain_desc.BufferDesc.Width;
        int height = m_swap_chain_desc.BufferDesc.Height;
        int stride = mapped_subresource.RowPitch;
        unsigned int size = m_swap_chain_desc.BufferDesc.Height * mapped_subresource.RowPitch;

        void *pixels = malloc(size);
        memcpy(pixels, mapped_subresource.pData, size);
        
        std::thread([&]() {
          // For now we assume that "comp" is RGBA, can cause issues later
          if (!stbi_write_png("screenshot.png", m_swap_chain_desc.BufferDesc.Width, m_swap_chain_desc.BufferDesc.Height, 4, pixels, mapped_subresource.RowPitch)) {
            Log(Log_Error, "<stbi_write_png> failed");
          }
        }).detach();

        RemoveState(State_Screenshot);
      }

      if (GetState(State_Recording)) {
        if (!m_video_manager.RecordFrame(mapped_subresource.pData, mapped_subresource.RowPitch)) {
          Log(Log_Error, "<RecordFrame> failed");
        }
      }

      m_device_context->Unmap(m_copy_texture, 0);
    }
  }

  // Draw overlay
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

  swap_chain->GetDesc(&m_swap_chain_desc); // TODO: Handle errors?
  CreateRenderTargetView(swap_chain); // TODO: Handle errors?

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

void GraphicsManager::Shutdown() {
  m_Present.Disable();
  m_ResizeBuffers.Disable();
  m_window_manager.UnhookFunctions();
  m_video_manager.Shutdown();
  
  if (m_device) m_device->Release();
  if (m_rtv) m_rtv->Release();
}