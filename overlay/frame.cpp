bool CaptureFrame(IDXGISwapChain *swap_chain, ID3D11Device *device,
                  ID3D11DeviceContext *device_context) {
  ID3D11Texture2D *back_buffer = nullptr;
  HRESULT success = swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
  if (FAILED(success)) {
    LOG(Log_Error, "<GetBuffer> failed, error = %d", success);
    return false;
  }

  success = CaptureTexture(device, device_context, back_buffer,
                           Global_FrameData.m_scratch_image);
  if (FAILED(success)) {
    LOG(Log_Error, "<CaptureTexture> failed, error = %d", success);
    return false;
  }

  return true;
}

bool SaveFrame(ID3D11DeviceContext *device_context) {
  HRESULT success =
      SaveToWICFile(Global_FrameData.m_scratch_image.GetImages(),
                    Global_FrameData.m_scratch_image.GetImageCount(),
                    WIC_FLAGS_NONE, GUID_ContainerFormatPng, L"Screenshot.png");
  if (FAILED(success)) {
    LOG(Log_Error, "<SaveToWICFile> failed, error = %d", success);
    return false;
  }

  return true;
}