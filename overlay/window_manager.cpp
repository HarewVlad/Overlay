WindowManager *WindowManager::m_instance = nullptr;

static LRESULT WINAPI WndProcStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
  return WindowManager::m_instance->WndProcHook(hwnd, msg, wparam, lparam);
}

LRESULT WINAPI WindowManager::WndProcHook(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
  if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
    return TRUE;
  }

  return CallWindowProc(m_original, hwnd, msg, wparam,
                        lparam);
}

bool WindowManager::HookFunctions() {
  m_original = (WNDPROC)GetWindowLongPtr(m_window, GWLP_WNDPROC);
  if (!m_original) {
    Log(Log_Error, "<GetWindowLongPtr> failed, error = %d", GetLastError());
    return false;
  }

  SetLastError(0);
  LONG_PTR result = SetWindowLongPtr(m_window, GWLP_WNDPROC, (LONG_PTR)WndProcStatic);
  DWORD error = GetLastError();
  if (!result && !error) {
    Log(Log_Error, "<SetWindowLongPtr> failed, error = %d", error);
    return false;
  }

  // TODO: Hook mouse, keyboard, ...

  return true;
}

bool WindowManager::UnhookFunctions() {
  Log(Log_Info, "Removing windows hooks ...");

  SetLastError(0);
  LONG_PTR result = SetWindowLongPtr(m_window, GWLP_WNDPROC, (LONG_PTR)m_original);
  DWORD error = GetLastError();
  if (!result && !error) {
    Log(Log_Error, "<SetWindowLongPtr> failed, error = %d", error);
    return false;
  }

  // TODO: Unhook mouse, keyboard, ...

  return true;
}