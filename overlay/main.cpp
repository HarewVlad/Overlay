#include "main.h"

// Imgui
#include "../vendor/include/imgui/imgui.cpp"
#include "../vendor/include/imgui/imgui_demo.cpp"
#include "../vendor/include/imgui/imgui_draw.cpp"
#include "../vendor/include/imgui/imgui_tables.cpp"
#include "../vendor/include/imgui/imgui_widgets.cpp"
#include "../vendor/include/imgui/imgui_impl_win32.cpp"
#include "../vendor/include/imgui/imgui_impl_dx11.cpp"

// Modules
#include "../log.cpp"
#include "../utils.cpp"
#include "hook.cpp"
#include "dx11.cpp"

// #ifndef _WINDLL
// // Test enviroments
// #include "test/directx11.cpp"
// #endif

HINSTANCE Global_Module;
HANDLE Global_Thread;

void InitializeOverlay(HINSTANCE instance) {
  InitializeLogger(Global_OverlayLogFilename);

  // Temp window
  const char *window_class_name = "temp_window_1935862";

  WNDCLASS wc = {};
  wc.style = CS_OWNDC;
  wc.lpfnWndProc = DefWindowProc;
  wc.hInstance = instance;
  wc.lpszClassName = window_class_name;

  if (!RegisterClass(&wc)) {
    Log("ERROR", "Unable to register class for temp window, error = %d", GetLastError());
    assert(0);
  }

  HWND temp_window =
      CreateWindowExA(0, window_class_name, window_class_name,
                     WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, 1, 1,
                     NULL, NULL, instance, NULL);

  if (!temp_window) {
    Log("ERROR", "Unable to create temp window, error = %d", GetLastError());
    assert(0);
  }

  // Detour graphic functions
  HMODULE dx11 = GetModuleHandle("d3d11.dll");
  if (dx11) {
    Log("INFO", "Found Directx11, hooking ...");

    DX11::Hook(temp_window);
  }

  MessageBoxA(NULL, "Injected!", "Injected", MB_OK);
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