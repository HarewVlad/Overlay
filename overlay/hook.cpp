bool Hook::Enable() {
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  DetourAttach((PVOID *)&m_original, m_fake);
  LONG error = DetourTransactionCommit();
  if (error != NO_ERROR) {
    Log(Log_Error, "<DetourTransactionCommit> failed, error = %d", error);
    return false;
  }

  return true;
}

bool Hook::Disable() {
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  DetourDetach((PVOID *)&m_original, m_fake);
  LONG error = DetourTransactionCommit();
  if (error != NO_ERROR) {
    Log(Log_Error, "<DetourTransactionCommit> failed, error = %d", error);
    return false;
  }

  return true;
}