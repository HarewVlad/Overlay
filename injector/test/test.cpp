bool TestInjection() {
  STARTUPINFO si = {};
  PROCESS_INFORMATION pi = {};

  if (!CreateProcessA(NULL, "target/main.exe", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
    LOG(Log_Error, "<CreateProcessA> failed, error = %d", GetLastError());
    return false;
  }

  Injector *injector = CreateInjector();

  if (!Inject(injector, pi.dwProcessId)) {
    LOG(Log_Error, "<Inject> failed");
    return false;
  }

  // TODO: Implement IPC and recieve code, signalling that we are injected

  return true;
}