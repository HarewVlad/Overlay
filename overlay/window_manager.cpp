WindowManager *WindowManager::m_instance = nullptr;

static LRESULT WINAPI WndProcStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
  return WindowManager::m_instance->WndProcHook(hwnd, msg, wparam, lparam);
}

LRESULT WINAPI WindowManager::WndProcHook(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
  if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
    return TRUE;
  }

  return CallWindowProc(m_WindowProcHook.m_original, hwnd, msg, wparam,
                        lparam);
}

bool WindowManager::HookFunctions() {
  WNDPROC wndproc = (WNDPROC)GetWindowLongPtr(m_window, GWLP_WNDPROC);
  if (!wndproc) {
    Log(Log_Error, "<GetWindowLongPtr> failed, error = %d", GetLastError());
    return false;
  }

  m_WindowProcHook.Initialize(wndproc, WndProcStatic);
  
  if (!m_WindowProcHook.Enable(m_window)) {
    Log(Log_Error, "Failed to enable window proc hook");
    return false;
  }

  // TODO: Hook mouse, keyboard, ...

  return true;
}

bool WindowManager::UnhookFunctions() {
  Log(Log_Info, "Removing windows hooks ...");

  if (!m_WindowProcHook.Disable(m_window)) {
    Log(Log_Error, "Failed to disable window proc hook");
    return false;
  }

  // TODO: Unhook mouse, keyboard, ...

  return true;
}