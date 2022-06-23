void InitializeLogger(const char *filename) {
  Global_Logger = std::ofstream(filename);

  if (!Global_Logger) {
    assert(0); // NOTE(Vlad): Lol, how to log this if no logger? =)
  }
}

void Log(const char *severity, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  Global_Logger << severity << ": ";

  while (*fmt != '\0') {
    if (*fmt == 'd') {
      int i = va_arg(args, int);
      Global_Logger << i;
    } else if (*fmt == 'c') {
      int c = va_arg(args, int);
      Global_Logger << c;
    } else if (*fmt == 'f') {
      double d = va_arg(args, double);
      Global_Logger << d;
    } else if (*fmt == 's') {
      const char *s = va_arg(args, const char *);
      Global_Logger << s;
    } else if (*fmt != '%') {
      Global_Logger << *fmt;
    }
    ++fmt;
  }

  Global_Logger << '\n';

  va_end(args);
}

void ShutdownLogger() {
  Global_Logger.close();
}