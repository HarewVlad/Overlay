Injector *CreateInjector() {
  Injector *result = new Injector {};

  HMODULE user32 = GetModuleHandle("user32.dll");
  HMODULE kernel32 = GetModuleHandle("kernel32.dll");

  result->m_LoadLibraryExA = (Injector::LoadLibraryExA)GetFunctionObfuscated(
      kernel32, "fHDObGK^LW^eQj", 0x87A678ABFB5CDB56ULL);

  result->m_SetWindowsHookEx = (Injector::SetWindowsHookEx)GetFunctionObfuscated(
      user32, "qG[zJCAL\\[`KFBlVe", 0x3DE7834312F54712ULL);

  result->m_library = (HINSTANCE)result->m_LoadLibraryExA(Global_OverlayName, NULL,
                                          DONT_RESOLVE_DLL_REFERENCES);

  return result;
}

struct EnumWindowsData {
  DWORD m_pid;
  HWND m_window;
};

BOOL IsMainWindow(HWND hWnd) {
  return GetWindow(hWnd, GW_OWNER) == (HWND)0 && IsWindowVisible(hWnd);
}

BOOL CALLBACK EnumWindowCallback(HWND hWnd, LPARAM lParam) {
  EnumWindowsData *data = (EnumWindowsData *)lParam;

  DWORD pid = 0;
  GetWindowThreadProcessId(hWnd, &pid);
  if (data->m_pid != pid || !IsMainWindow(hWnd)) return TRUE;
  data->m_window = hWnd;

  return FALSE;
}

HWND FindMainWindow(DWORD pid) {
  EnumWindowsData data;
  data.m_pid = pid;
  data.m_window = NULL;
  EnumWindows(EnumWindowCallback, (LPARAM)&data);
  return data.m_window;
}

bool Inject(Injector *injector, DWORD pid) {
  HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
  if (!process) {
    Log("ERROR", "<OpenProcess> failed, error = %d", GetLastError());
    return false;
  }

  HOOKPROC hook_proc = (HOOKPROC)GetProcAddress(injector->m_library, "GetMessageProc");
  if (!hook_proc) {
    Log("ERROR", "<GetProcAddress> failed, error = %d", GetLastError());
    return false;
  }

  HWND main_window = NULL;
  while ((main_window = FindMainWindow(pid)) == NULL) {
    Sleep(10);
  }

  DWORD thread = GetWindowThreadProcessId(main_window, NULL);

  HHOOK hook =
      injector->m_SetWindowsHookEx(WH_GETMESSAGE, hook_proc, injector->m_library, thread);
  if (!hook) {
    Log("ERROR", "<m_SetWindowsHookEx> failed, error = %d", GetLastError());
    return false;
  }
  
  Sleep(500); // NOTE(Vlad): Small sleep helps
  PostThreadMessage(thread, WM_USER + 500, NULL, (LPARAM)hook);

  return true;
}