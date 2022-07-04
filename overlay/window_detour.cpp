LRESULT WINAPI WndProcHook(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
  if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
    return TRUE;
  }

  return CallWindowProc(Global_WindowData.m_WindowProcHook->m_original, hwnd, msg, wparam,
                        lparam);

  return TRUE;
}

bool WindowHook(HWND window) {
  WNDPROC original_wndproc = (WNDPROC)GetWindowLongPtr(window, GWLP_WNDPROC); // TODO: Check for possible error later

  Global_WindowData.m_WindowProcHook = CreateWindowProcHook(original_wndproc, WndProcHook);

  if (!EnableWindowProcHook(Global_WindowData.m_WindowProcHook, window)) {
    LOG(Log_Error, "<EnableWindowProcHook> failed");
    return false;
  }

  // TODO: Hook mouse, keyboard, ...

  Global_WindowData.m_window = window;

  return true;
}

void WindowShutdown() {
  if (!RemoveWindowProcHook(Global_WindowData.m_WindowProcHook, Global_WindowData.m_window)) {
    LOG(Log_Error, "<RemoveWindowProcHook> failed");
    // NOTE(Vlad): Should not be huge deal for now
  }

  delete Global_WindowData.m_WindowProcHook;
}