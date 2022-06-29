std::ofstream Global_Logger;

enum LogLevel {
  Log_Error,
  Log_Info
};

const char *Global_LogLevelToString[] = {"ERROR", "INFO"};

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

bool InitializeLogger(const char *filename);
void Log(int severity, const char *fmt, ...);
void ShutdownLogger();