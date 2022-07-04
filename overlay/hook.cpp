Hook *CreateHook(PVOID *original, PVOID fake) {
  Hook *result = new Hook {};

  result->m_original = original;
  result->m_fake = fake;

  return result;
}

bool EnableHook(Hook *hook) {
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  DetourAttach((PVOID *)&hook->m_original, hook->m_fake);
  LONG error = DetourTransactionCommit();
  if (error != NO_ERROR) {
    LOG(Log_Error, "<DetourTransactionCommit> failed, error = %d", error);
    return false;
  }

  return true;
}

bool RemoveHook(Hook *hook) {
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  DetourDetach((PVOID *)&hook->m_original, hook->m_fake);
  LONG error = DetourTransactionCommit();
  if (error != NO_ERROR) {
    LOG(Log_Error, "<DetourTransactionCommit> failed, error = %d", error);
    return false;
  }

  return true;
}