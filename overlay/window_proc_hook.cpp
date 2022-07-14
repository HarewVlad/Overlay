bool WindowProcHook::Enable(HWND window) {
  SetLastError(0);
  LONG_PTR result = SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)m_fake);
  DWORD error = GetLastError();
  if (!result && !error) {
    Log(Log_Error, "<SetWindowLongPtr> failed, error = %d", error);
    return false;
  }

  return true;
}

bool WindowProcHook::Disable(HWND window) {
  SetLastError(0);
  LONG_PTR result = SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)m_original);
  DWORD error = GetLastError();
  if (!result && !error) {
    Log(Log_Error, "<SetWindowLongPtr> failed, error = %d", error);
    return false;
  }

  return true;
}