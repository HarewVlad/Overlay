void InitializeLogger(const char *filename) {
  Global_Logger = std::ofstream(filename);

  if (!Global_Logger) {
    assert(0 && "Unable to initialize logger"); // NOTE(Vlad): Lol, how to log this if no logger? =)
  }
}

void Log(const char *severity, const char *fmt, ...) {
  Global_Logger << severity << ": ";

  va_list args;
  va_start(args, fmt);

  char buffer[1024];
  int n = vsnprintf(buffer, 1024, fmt, args);

  va_end(args);

  Global_Logger << buffer << '\n';
}

void ShutdownLogger() {
  Global_Logger.close();
}