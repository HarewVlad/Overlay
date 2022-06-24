#include "main.h"

// Imgui
#include "vendor/include/imgui/imgui.cpp"
#include "vendor/include/imgui/imgui_demo.cpp"
#include "vendor/include/imgui/imgui_draw.cpp"
#include "vendor/include/imgui/imgui_tables.cpp"
#include "vendor/include/imgui/imgui_widgets.cpp"
#include "vendor/include/imgui/imgui_impl_win32.cpp"
#include "vendor/include/imgui/imgui_impl_dx11.cpp"

// Modules
#include "log.cpp"
#include "utils.cpp"
#include "hook.cpp"

#ifndef _WINDLL
// Test enviroments
#include "test/directx11.cpp"
#endif

HMODULE Global_Module;
HANDLE Global_Thread;

void InitializeOverlay() {
  #ifdef _X86_
  const char *filename = "overlay_x86.txt";
  #elif defined _X64_
  const char *filename = "overlay_x64.txt";
  #endif

  InitializeLogger(filename);
}

void ShutdownOverlay() {
  ShutdownLogger();

  WaitForSingleObject(Global_Thread, INFINITE);
  FreeLibraryAndExitThread(Global_Module, 0);
}

void EjectOverlay() {
  CloseHandle(CreateThread(
    NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(ShutdownOverlay), 0, 0,
    0));
}

int main() {
  #ifdef _X86_
  const char *filename = "overlay_injector_x86.txt";
  #elif defined _X64_
  const char *filename = "overlay_injector_x64.txt";
  #endif

  InitializeLogger(filename);

  ShutdownLogger();
}

#ifdef _WINDLL
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
        Log("INFO", "Exiting ...");
        break;
    }
    return TRUE;
  }

  LRESULT CALLBACK GetMessageProc(int code, WPARAM wparam, LPARAM lparam) {
  #pragma EXPORT
    MSG* msg = (MSG*)lparam;
    if (msg->message == (WM_USER + 500)) { // NOTE(Vlad): Yes
      auto unhook_windows_hook_ex =
          (tUnhookWindowsHookEx)GetFunctionObfuscated(
              GetModuleHandleA("User32.dll"), "zBOKD@|MDEA\\^o@GTyO",
              0x7FED328401A17FD5ULL);

      unhook_windows_hook_ex((HHOOK)msg->lParam);
    }

    return CallNextHookEx(NULL, code, wparam, lparam);
  }
#endif