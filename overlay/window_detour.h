struct WindowData {
  HWND m_window;

  WindowProcHook *m_WindowProcHook;
};

WindowData Global_WindowData;
  
bool WindowHook(HWND window);
void WindowShutdown();