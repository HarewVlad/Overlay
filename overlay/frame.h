struct FrameData {
  DirectX::ScratchImage m_scratch_image;
};

static FrameData Global_FrameData;

bool CaptureFrame(IDXGISwapChain *swap_chain, ID3D11Device *device, ID3D11DeviceContext *device_context);
bool SaveFrame(ID3D11DeviceContext *device_context);