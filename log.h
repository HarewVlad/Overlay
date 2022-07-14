std::ofstream Global_Logger;

enum LogLevel { Log_Error, Log_Info };

#define __FILENAME__                                                           \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define Log(Level, Format, ...)                                                \
  {                                                                            \
    if (Level == Log_Error)                                                    \
      Global_Logger << "ERROR: (" << __FILENAME__ << ", " << __LINE__          \
                    << ") - ";                                                 \
    else                                                                       \
      Global_Logger << "INFO: ";                                               \
    LogInternal(Format, __VA_ARGS__);                                          \
  }

bool InitializeLogger(const char *filename);
void LogInternal(const char *fmt, ...);
void ShutdownLogger();