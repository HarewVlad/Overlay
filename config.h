#ifdef _X86_
  const char *Global_OverlayName = "overlay_x86.dll";
  const char *Global_OverlayLogFilename = "overlay_x86.txt";
  const char *Global_InjectorLogFilename = "injector_x86.txt";
#elif defined _X64_
  const char *Global_OverlayName = "overlay_x64.dll";
  const char *Global_OverlayLogFilename = "overlay_x64.txt";
  const char *Global_InjectorLogFilename = "injector_x64.txt";
#endif