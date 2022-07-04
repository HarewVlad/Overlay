static HINSTANCE Global_Module;
static HANDLE Global_Thread;

void StartOverlay(HINSTANCE instance);
bool InitializeOverlay(HINSTANCE instance);
void ShutdownOverlay();
void EjectOverlay();