static HINSTANCE Global_Module;
static HANDLE Global_Thread;

struct Overlay {
  bool Initialize(HINSTANCE instance);
  void Shutdown();

  GraphicsManager m_graphics_manager;
};

void RunOverlay(HINSTANCE instance);
void EjectOverlay();