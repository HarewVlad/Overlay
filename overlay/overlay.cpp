bool InitializeOverlay(HINSTANCE instance) {
  if (!InitializeLogger(Global_OverlayLogFilename)) {
    return false;
  }

  // Temp window
  const char *window_class_name = "temp_window_1935862";

  WNDCLASS wc = {};
  wc.style = CS_OWNDC;
  wc.lpfnWndProc = DefWindowProc;
  wc.hInstance = instance;
  wc.lpszClassName = window_class_name;

  if (!RegisterClass(&wc)) {
    LOG(Log_Error, "<RegisterClass> failed, error = %d", GetLastError());
    return false;
  }

  HWND temp_window =
      CreateWindowExA(0, window_class_name, window_class_name,
                     WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, 1, 1,
                     NULL, NULL, instance, NULL);

  if (!temp_window) {
    LOG(Log_Error, "<CreateWindowExA> failed, error = %d", GetLastError());
    return false;
  }

  // Detour graphic functions
  HMODULE dx11 = GetModuleHandle("d3d11.dll");
  if (dx11) {
    LOG(Log_Info, "Found Directx11, hooking ...");

    if (!Dx11Hook(temp_window)) {
      LOG(Log_Info, "<Dx11Hook> failed");
      return false;
    }
  }

  return true;
}

void StartOverlay(HINSTANCE instance) {
  if (!InitializeOverlay(instance)) {
    LOG(Log_Error, "<InitializeOverlay> error");
    ShutdownOverlay();
    return;
  }

  while (!GetState(State_Close)) {
    Sleep(10);
  }

  EjectOverlay();
}

void ShutdownOverlay() {
  WindowShutdown();
  Dx11Shutdown();
  
  WaitForSingleObject(Global_Thread, INFINITE);
  FreeLibraryAndExitThread(Global_Module, 0);
}

void EjectOverlay() {
  CloseHandle(CreateThread(
    NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(ShutdownOverlay), 0, 0,
    0));
}