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
    Log("ERROR", "<RegisterClass> failed, error = %d", GetLastError());
    ShutdownOverlay();
    return;
  }

  HWND temp_window =
      CreateWindowExA(0, window_class_name, window_class_name,
                     WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, 1, 1,
                     NULL, NULL, instance, NULL);

  if (!temp_window) {
    Log("ERROR", "<CreateWindowExA> failed, error = %d", GetLastError());
    ShutdownOverlay();
    return;
  }

  // Detour graphic functions
  HMODULE dx11 = GetModuleHandle("d3d11.dll");
  if (dx11) {
    Log("INFO", "Found Directx11, hooking ...");

    if (!Dx11Hook(temp_window)) {
      Log("INFO", "<Dx11Hook> failed");
      ShutdownOverlay();
      return;
    }
  }

  // MessageBoxA(NULL, "Injected!", "Injected", MB_OK);
}

void ShutdownOverlay() {
  ShutdownLogger();

  // Disable graphics hooks
  // Dx11Shutdown();

  // WaitForSingleObject(Global_Thread, INFINITE);
  // FreeLibraryAndExitThread(Global_Module, 0);
}

void EjectOverlay() {
  CloseHandle(CreateThread(
    NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(ShutdownOverlay), 0, 0,
    0));
}