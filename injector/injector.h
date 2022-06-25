struct Injector {
  typedef HMODULE(WINAPI *tLoadLibraryExA)(LPCSTR, HANDLE, DWORD);
  typedef HHOOK(WINAPI *tSetWindowsHookEx)(int, HOOKPROC, HINSTANCE, DWORD);

  tLoadLibraryExA m_LoadLibraryExA;
  tSetWindowsHookEx m_SetWindowsHookEx;
  HINSTANCE m_library;
};

Injector *CreateInjector();
void Inject(Injector *injector, DWORD pid);