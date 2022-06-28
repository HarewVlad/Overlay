std::ofstream Global_Logger;

void InitializeLogger(const char *filename);
void Log(const char *severity, const char *fmt, ...);
void ShutdownLogger();