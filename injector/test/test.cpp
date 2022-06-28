void TestInjection() {
  STARTUPINFO si = {};
  PROCESS_INFORMATION pi = {};

  if (!CreateProcessA(NULL, "target/main.exe", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
    Log("ERROR", "Unable to create test process");
    assert(0);
  }

  Injector *injector = CreateInjector();
  Inject(injector, pi.dwProcessId);
}