bool Overlay::Initialize(HINSTANCE instance) {
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
    Log(Log_Error, "Failed to register dummy class, error = %d", GetLastError());
    return false;
  }

  HWND temp_window =
      CreateWindowExA(0, window_class_name, window_class_name,
                     WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, 1, 1,
                     NULL, NULL, instance, NULL);

  if (!temp_window) {
    Log(Log_Error, "Failed to create dummy window, error = %d", GetLastError());
    return false;
  }

  // Detour graphic functions
  HMODULE dx11 = GetModuleHandle("d3d11.dll");
  if (dx11) {
    Log(Log_Info, "Found Directx11, hooking ...");
    if (!m_graphics_manager.HookFunctions(temp_window)) {
      Log(Log_Info, "Failed to hook graphics functions");
      return false;
    }
  }

  DestroyWindow(temp_window);
  UnregisterClass(window_class_name, instance);

  return true;
}

void RunOverlay(HINSTANCE instance) {
  Overlay overlay;

  if (!overlay.Initialize(instance)) {
    Log(Log_Error, "Failed to initialize overlay");
  } else {
    while (!GetState(State_Close)) {
      Sleep(10);
    }
  }

  overlay.m_graphics_manager.Shutdown();

  EjectOverlay();
}

void Eject() {
  WaitForSingleObject(Global_Thread, INFINITE);
  FreeLibraryAndExitThread(Global_Module, 0);
}

void EjectOverlay() {
  CloseHandle(CreateThread(
    NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(Eject), 0, 0,
    0));
}