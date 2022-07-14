struct WindowProcHook {
  void Initialize(WNDPROC original, WNDPROC fake) {
    m_original = original;
    m_fake = fake;
  };

  bool Enable(HWND window);
  bool Disable(HWND window);

  WNDPROC m_original;
  WNDPROC m_fake;
};