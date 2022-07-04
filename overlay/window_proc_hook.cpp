WindowProcHook *CreateWindowProcHook(WNDPROC original, WNDPROC fake) {
  WindowProcHook *result = new WindowProcHook {};

  result->m_original = original;
  result->m_fake = fake;

  return result;
}

bool EnableWindowProcHook(WindowProcHook *window_proc_hook, HWND window) {
  SetLastError(0);
  LONG_PTR result = SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)window_proc_hook->m_fake);
  DWORD error = GetLastError();
  if (!result && !error) {
    LOG(Log_Error, "<SetWindowLongPtr> failed, error = %d", error);
    return false;
  }

  return true;
}

bool RemoveWindowProcHook(WindowProcHook *window_proc_hook, HWND window) {
  SetLastError(0);
  LONG_PTR result = SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)window_proc_hook->m_original);
  DWORD error = GetLastError();
  if (!result && !error) {
    LOG(Log_Error, "<SetWindowLongPtr> failed, error = %d", error);
    return false;
  }

  return true;
}