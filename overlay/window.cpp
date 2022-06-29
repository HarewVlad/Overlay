LRESULT WINAPI WndProcHook(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
  if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
    return TRUE;
  }

  return CallWindowProc(Global_Window.m_WindowProcHook->m_original, hwnd, msg, wparam,
                        lparam);

  return TRUE;
}

bool WindowHook(HWND window) {
  WNDPROC original_wndproc = (WNDPROC)GetWindowLongPtr(window, GWLP_WNDPROC); // TODO: Check for possible error later

  Global_Window.m_WindowProcHook = CreateWindowProcHook(original_wndproc, WndProcHook);

  if (!EnableWindowProcHook(Global_Window.m_WindowProcHook, window)) {
    Log(Log_Error, "<EnableWindowProcHook> failed");
    return false;
  }

  // TODO: Hook mouse, keyboard, ...

  return true;
}