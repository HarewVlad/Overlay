struct Window {
  WindowProcHook *m_WindowProcHook;
};

Window Global_Window;

bool WindowHook(HWND window);