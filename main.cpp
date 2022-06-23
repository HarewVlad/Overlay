#include "main.h"

#include "hook.cpp"

HMODULE Global_Module;
HANDLE Global_Thread;

void InitializeOverlay() {

}

void UninitializeOverlay() {
  WaitForSingleObject(Global_Thread, INFINITE);
  FreeLibraryAndExitThread(Global_Module, 0);
}

void EjectOverlay() {
  CloseHandle(CreateThread(
    NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(UninitializeOverlay), 0, 0,
    0));
}

int main() {
  Hook *hook = CreateHook(nullptr, nullptr);
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) {
  DisableThreadLibraryCalls(static_cast<HMODULE>(instance));

  switch (reason) {
    case DLL_PROCESS_ATTACH:
      wchar_t filename[MAX_PATH];
      GetModuleFileNameW(instance, filename, MAX_PATH);
      Global_Module = LoadLibraryW(filename);

      Global_Thread = CreateThread(
          NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(InitializeOverlay), (LPVOID)instance,
          0, 0);

      break;
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      break;
    case DLL_PROCESS_DETACH:
      std::cout << "Exiting ..."; // NOTE(Vlad): Need to log to the file
      break;
  }
  return TRUE;
}