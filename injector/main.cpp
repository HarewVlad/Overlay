#include "main.h"

// Imgui
// #include "vendor/include/imgui/imgui.cpp"
// #include "vendor/include/imgui/imgui_demo.cpp"
// #include "vendor/include/imgui/imgui_draw.cpp"
// #include "vendor/include/imgui/imgui_tables.cpp"
// #include "vendor/include/imgui/imgui_widgets.cpp"
// #include "vendor/include/imgui/imgui_impl_win32.cpp"
// #include "vendor/include/imgui/imgui_impl_dx11.cpp"

// Modules
#include "../log.cpp"
#include "../utils.cpp"
#include "injector.cpp"

// Test
#include "test.cpp"

int main(int argc, char **argv) {
  if (!InitializeLogger(Global_InjectorLogFilename)) {
    return 1;
  }

  // NOTE(Vlad): For now assume that if no args, than it is in test mode
  if (argc < 2) {
    Log(Log_Info, "Tests...");

    // if (!TestInjection()) Log(Log_Error, "Test injection failed");
    if (!TestInjection()) Log(Log_Info, "Injection test failed");

    // TODO: Add more tests later

    Log(Log_Info, "Tests done!");
  } else {
    DWORD pid = atoi(argv[1]);

    Log(Log_Info, "Connecting to : %d", pid);

    Injector injector;
    injector.Initialize();

    if (!injector.Inject(pid)) {
      Log(Log_Error, "Failed to inject into process");
    }
  }

  ShutdownLogger();
}