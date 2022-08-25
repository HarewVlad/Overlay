struct GraphicsManager {
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

  GraphicsManager() { m_instance = this; };

  bool HookFunctions(HWND window);
  bool Initialize(IDXGISwapChain *swap_chain);
  bool CreateRenderTargetView(IDXGISwapChain *swap_chain);
  void Shutdown();

  HRESULT WINAPI PresentHook(IDXGISwapChain *swap_chain, UINT sync_interval,
                           UINT flags);
  HRESULT WINAPI ResizeBuffersHook(IDXGISwapChain *swap_chain, UINT buffer_count,
                                 UINT width, UINT height,
                                 DXGI_FORMAT new_format,
                                 UINT swap_chain_flags);

  static GraphicsManager *m_instance;

  WindowManager m_window_manager;
  VideoManager m_video_manager;
  Hook m_Present;
  Hook m_ResizeBuffers;

  ID3D11RenderTargetView *m_rtv;
  ID3D11Device *m_device;
  ID3D11DeviceContext *m_device_context;
  ID3D11Texture2D *m_copy_texture;
  DXGI_FORMAT m_format;
  UINT m_width;
  UINT m_height;
  bool m_multisampled;
};