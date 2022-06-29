bool InitializeLogger(const char *filename) {
  Global_Logger = std::ofstream(filename);

  if (!Global_Logger) {
    MessageBoxA(NULL, "Unable to initialize logger", "Error", MB_OK);// NOTE(Vlad): Lol, how to log
                                                // this if no logger? =)
    return false;
  }
  
  return true;
}

void Log(int severity, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  char buffer[1024];
  int n = vsnprintf(buffer, 1024, fmt, args);

  va_end(args);

  switch (severity) {
  case Log_Error:
    Global_Logger << Global_LogLevelToString[severity] << ": (" << __FILENAME__ << ", " << __LINE__ << ") - ";
    break;
  case Log_Info:
    Global_Logger << Global_LogLevelToString[severity] << ": ";
    break;
  default:
    assert(0 && "Wrong severity level argument");
    break;
  }

  Global_Logger << buffer << '\n';
}

void ShutdownLogger() { Global_Logger.close(); }