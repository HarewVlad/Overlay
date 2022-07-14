#include "main.h"

// DirectXTex
#include "../vendor/include/directxtex/BC.cpp"
#include "../vendor/include/directxtex/BC4BC5.cpp"
#include "../vendor/include/directxtex/BC6HBC7.cpp"
#include "../vendor/include/directxtex/DirectXTexMipmaps.cpp"
#include "../vendor/include/directxtex/DirectXTexUtil.cpp"
#include "../vendor/include/directxtex/DirectXTexImage.cpp"
#include "../vendor/include/directxtex/DirectXTexConvert.cpp"
#include "../vendor/include/directxtex/DirectXTexCompress.cpp"
#include "../vendor/include/directxtex/DirectXTexD3D11.cpp"
#include "../vendor/include/directxtex/DirectXTexWIC.cpp"

// Imgui
#include "../vendor/include/imgui/imgui.cpp"
#include "../vendor/include/imgui/imgui_demo.cpp"
#include "../vendor/include/imgui/imgui_draw.cpp"
#include "../vendor/include/imgui/imgui_tables.cpp"
#include "../vendor/include/imgui/imgui_widgets.cpp"
#include "../vendor/include/imgui/imgui_impl_win32.cpp"
#include "../vendor/include/imgui/imgui_impl_dx11.cpp"

// Modules
#include "state.cpp"
#include "../log.cpp"
#include "../utils.cpp"
#include "video.cpp"
#include "window_proc_hook.cpp"
#include "window_manager.cpp"
#include "hook.cpp"
#include "graphics_manager.cpp"
#include "overlay.cpp"
#include "gui.cpp"

#ifndef _WINDLL
#include "test/dx11.cpp"
#endif

#ifdef _WINDLL
  BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) {
    DisableThreadLibraryCalls(static_cast<HMODULE>(instance));

    switch (reason) {
      case DLL_PROCESS_ATTACH:
        wchar_t filename[MAX_PATH];
        GetModuleFileNameW(instance, filename, MAX_PATH);
        Global_Module = LoadLibraryW(filename);

        Global_Thread = CreateThread(
            NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(RunOverlay), (LPVOID)instance,
            0, 0);

        break;
      case DLL_THREAD_ATTACH:
        break;
      case DLL_THREAD_DETACH:
        break;
      case DLL_PROCESS_DETACH:
        Log(Log_Info, "Exiting ...");

        ShutdownLogger();
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
#else
int main() {
  Test::DX11::Initialize();
}
#endif