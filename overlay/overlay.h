static HINSTANCE Global_Module;
static HANDLE Global_Thread;

struct Overlay {
  bool Initialize(HINSTANCE instance);

  GraphicsManager m_graphics_manager;
};

void RunOverlay(HINSTANCE instance);
void EjectOverlay();