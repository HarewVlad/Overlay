bool InitializeFrame(IDXGISwapChain *swap_chain, ID3D11Device *device) {
  DXGI_SWAP_CHAIN_DESC swap_chain_desc;
  HRESULT success = swap_chain->GetDesc(&swap_chain_desc);
  if (FAILED(success)) {
    LOG(Log_Error, "<GetDesc> failed, error = %d", success);
    return false;
  }

  // NOTE(Vlad): Texture info
  Global_Frame.m_format = swap_chain_desc.BufferDesc.Format;
  Global_Frame.m_multisampled = swap_chain_desc.SampleDesc.Count > 1;
  Global_Frame.m_width = swap_chain_desc.BufferDesc.Width;
  Global_Frame.m_height = swap_chain_desc.BufferDesc.Height;

  // NOTE(Vlad): Create texture
  D3D11_TEXTURE2D_DESC texture_desc = {};
  texture_desc.Width = swap_chain_desc.BufferDesc.Width;
  texture_desc.Height = swap_chain_desc.BufferDesc.Height;
  texture_desc.Format = swap_chain_desc.BufferDesc.Format;
  texture_desc.MipLevels = 1;
  texture_desc.ArraySize = 1;
  texture_desc.SampleDesc.Count = 1;
  texture_desc.Usage = D3D11_USAGE_STAGING;
  texture_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

  success = device->CreateTexture2D(&texture_desc, nullptr, &Global_Frame.m_texture);
  if (FAILED(success)) {
    LOG(Log_Error, "<CreateTexture2D> failed, error = %d", success);
    return false;
  }

  return true;
}

bool CaptureFrame(IDXGISwapChain *swap_chain, ID3D11DeviceContext *device_context) {
  ID3D11Texture2D *back_buffer = nullptr;
  HRESULT success = swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
  if (FAILED(success)) {
    LOG(Log_Error, "<GetBuffer> failed, error = %d", success);
    return false;
  }

  if (!Global_Frame.m_multisampled) {
    device_context->CopyResource(Global_Frame.m_texture, back_buffer);
  } else {
    device_context->ResolveSubresource(Global_Frame.m_texture, 0, back_buffer, 0, Global_Frame.m_format);
  }

  back_buffer->Release();

  // TODO: Save to file?
  // D3DX11SaveTextureToFile();

  return true;
}