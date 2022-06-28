struct Injector {
  typedef HMODULE(WINAPI *LoadLibraryExA)(LPCSTR, HANDLE, DWORD);
  typedef HHOOK(WINAPI *SetWindowsHookEx)(int, HOOKPROC, HINSTANCE, DWORD);

  LoadLibraryExA m_LoadLibraryExA;
  SetWindowsHookEx m_SetWindowsHookEx;
  HINSTANCE m_library;
};

Injector *CreateInjector();
bool Inject(Injector *injector, DWORD pid);