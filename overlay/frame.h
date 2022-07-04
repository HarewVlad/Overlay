struct Frame {
  ID3D11Texture2D *m_texture;
  unsigned int m_width;
  unsigned int m_height;
  DXGI_FORMAT m_format;
  bool m_multisampled;
};

static Frame Global_Frame;

bool InitializeFrame(IDXGISwapChain *swap_chain, ID3D11Device *device);
bool CaptureFrame(IDXGISwapChain *swap_chain, ID3D11DeviceContext *device_context);