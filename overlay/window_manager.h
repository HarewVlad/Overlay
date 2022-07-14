struct WindowManager {
  WindowManager() { m_instance = this; };

  void Initialize(HWND window) { m_window = window; }
  bool HookFunctions();
  bool UnhookFunctions();

  LRESULT WINAPI WndProcHook(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

  static WindowManager *m_instance;

  HWND m_window;
  WindowProcHook m_WindowProcHook;
};