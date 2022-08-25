void ImGuiInitializeWin32(HWND window) {
  ImGui::CreateContext();
  ImGui_ImplWin32_Init(window);

  // NOTE(Vlad): Don't need to save overlay gui elements positions
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.IniFilename = NULL;
}

void ImGuiInitializeGraphics(ID3D11Device *device, ID3D11DeviceContext *device_context) {
  ImGui_ImplDX11_Init(device, device_context);
}

void ImGuiBegin() {
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
}

void ImGuiEnd() {
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiDrawOverlay() {
  static int corner = 0;
  ImGuiIO& io = ImGui::GetIO();
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
  if (corner != -1)
  {
      const float PAD = 10.0f;
      const ImGuiViewport* viewport = ImGui::GetMainViewport();
      ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
      ImVec2 work_size = viewport->WorkSize;
      ImVec2 window_pos, window_pos_pivot;
      window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
      window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
      window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
      window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
      ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
      window_flags |= ImGuiWindowFlags_NoMove;
  }
  ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
  if (ImGui::Begin("Example: Simple overlay", NULL, window_flags))
  {
      IMGUI_DEMO_MARKER("Examples/Simple Overlay");
      ImGui::Text("Simple overlay\n" "in the corner of the screen.\n" "(right-click to change position)");

      if (ImGui::Button("Screenshot")) {
        Log(Log_Info, "Screenshot...");

        SetState(State_Screenshot);
      }

      if (ImGui::Button("Start recording")) {
        Log(Log_Info, "Start video recording...");

        SetState(State_StartRecording);
      }

      if (ImGui::Button("End recording")) {
        Log(Log_Info, "End video recording...");

        SetState(State_EndRecording);
      }

      if (ImGui::Button("Exit")) {
        Log(Log_Info, "Exit requested");

        SetState(State_Close);
      }

      ImGui::Separator();
      if (ImGui::IsMousePosValid())
          ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
      else
          ImGui::Text("Mouse Position: <invalid>");
      if (ImGui::BeginPopupContextWindow())
      {
          if (ImGui::MenuItem("Custom",       NULL, corner == -1)) corner = -1;
          if (ImGui::MenuItem("Top-left",     NULL, corner == 0)) corner = 0;
          if (ImGui::MenuItem("Top-right",    NULL, corner == 1)) corner = 1;
          if (ImGui::MenuItem("Bottom-left",  NULL, corner == 2)) corner = 2;
          if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
          ImGui::EndPopup();
      }
  }
  ImGui::End();
}

void ImGuiDraw() {
  ImGui::NewFrame();

  ImGuiDrawOverlay();

  ImGui::Render();
}