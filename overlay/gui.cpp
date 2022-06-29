void ImGuiInitializeWin32(HWND window) {
  ImGui::CreateContext();
  ImGui_ImplWin32_Init(window);
}

void ImGuiInitializeDx11(ID3D11Device *device, ID3D11DeviceContext *device_context) {
  ImGui_ImplDX11_Init(device, device_context);
}

void ImGuiBeginDx11() {
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
}

void ImGuiEndDx11() {
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiDraw() {
  ImGui::NewFrame();

  ImGui::Begin("Hello", NULL);

  ImGui::Text("Overlay!");

  ImGui::End();

  ImGui::Render();
}