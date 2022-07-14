struct Injector {
  typedef HMODULE(WINAPI *LoadLibraryExA)(LPCSTR, HANDLE, DWORD);
  typedef HHOOK(WINAPI *SetWindowsHookEx)(int, HOOKPROC, HINSTANCE, DWORD);

  bool Initialize();
  bool Inject(DWORD pid);

  LoadLibraryExA m_LoadLibraryExA;
  SetWindowsHookEx m_SetWindowsHookEx;
  HINSTANCE m_library;
};