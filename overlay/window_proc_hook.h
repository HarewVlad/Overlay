struct WindowProcHook {
  WNDPROC m_original;
  WNDPROC m_fake;
};

WindowProcHook *CreateWindowProcHook(WNDPROC original, WNDPROC fake);
bool EnableWindowProcHook(WindowProcHook *window_proc_hook, HWND window);
bool RemoveWindowProcHook(WindowProcHook *window_proc_hook, HWND window);