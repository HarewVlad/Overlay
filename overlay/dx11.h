struct Dx11 {
  typedef HRESULT(WINAPI *Present)(IDXGISwapChain *, UINT, UINT);
  typedef HRESULT(WINAPI *ResizeBuffers)(IDXGISwapChain *, UINT, UINT, UINT,
                                          DXGI_FORMAT, UINT);
  typedef HRESULT(WINAPI *CreateDeviceAndSwapChain)(
      _In_opt_ IDXGIAdapter *pAdapter, D3D_DRIVER_TYPE DriverType,
      HMODULE Software, UINT Flags,
      _In_reads_opt_(FeatureLevels) CONST D3D_FEATURE_LEVEL *pFeatureLevels,
      UINT FeatureLevels, UINT SDKVersion,
      _In_opt_ CONST DXGI_SWAP_CHAIN_DESC *pSwapChainDesc,
      _COM_Outptr_opt_ IDXGISwapChain **ppSwapChain,
      _COM_Outptr_opt_ ID3D11Device **ppDevice,
      _Out_opt_ D3D_FEATURE_LEVEL *pFeatureLevel,
      _COM_Outptr_opt_ ID3D11DeviceContext **ppImmediateContext);

  ID3D11RenderTargetView *m_rtv;
  ID3D11Device *m_device;
  ID3D11DeviceContext *m_device_context;

  Hook *m_Present;
  Hook *m_ResizeBuffers;
};

Dx11 Global_Dx11;

bool Dx11Hook(HWND window);
void Dx11Shutdown();
