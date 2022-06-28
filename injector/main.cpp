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
#include "test/test.cpp"

int main(int argc, char **argv) {
  InitializeLogger(Global_InjectorLogFilename);

  // NOTE(Vlad): For now assume that if no args, than it is in test mode
  if (argc < 2) {
    Log("INFO", "Test injection ...");
    TestInjection();
    Log("INFO", "Test injection done!");
  } else {
    DWORD pid = atoi(argv[1]);

    Injector *injector = CreateInjector();
    Inject(injector, pid);
  }

  ShutdownLogger();
}