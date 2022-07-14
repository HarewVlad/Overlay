bool SaveScreenshot(IDXGISwapChain *swap_chain, ID3D11Device *device,
                  ID3D11DeviceContext *device_context) {
  ID3D11Texture2D *back_buffer = nullptr;
  HRESULT success = swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
  if (FAILED(success)) {
    Log(Log_Error, "<GetBuffer> failed, error = %d", success);
    return false;
  }

  ScratchImage scratch_image;
  success = CaptureTexture(device, device_context, back_buffer,
                           scratch_image);

  if (FAILED(success)) {
    Log(Log_Error, "<CaptureTexture> failed, error = %d", success);
    return false;
  }

  success =
      SaveToWICFile(scratch_image.GetImages(),
                    scratch_image.GetImageCount(),
                    WIC_FLAGS_NONE, GUID_ContainerFormatPng, L"Screenshot.png");
  if (FAILED(success)) {
    Log(Log_Error, "<SaveToWICFile> failed, error = %d", success);
    return false;
  }

  return true;
}