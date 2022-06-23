std::ofstream Global_Logger;

void InitializeLogger(const char *filename);
void Log(const char *fmt, ...);
void ShutdownLogger();