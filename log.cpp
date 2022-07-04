bool InitializeLogger(const char *filename) {
  Global_Logger = std::ofstream(filename);

  if (!Global_Logger) {
    MessageBoxA(NULL, "Unable to initialize logger", "Error", MB_OK);// NOTE(Vlad): Lol, how to log
                                                // this if no logger? =)
    return false;
  }
  
  return true;
}

void Log(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  char buffer[1024];
  int n = vsnprintf(buffer, 1024, fmt, args);

  va_end(args);

  Global_Logger << buffer << '\n';
}

void ShutdownLogger() { Global_Logger.close(); }