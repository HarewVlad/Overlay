Hook *CreateHook(PVOID *original, PVOID fake) {
  Hook *result = new Hook {};

  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  DetourAttach((PVOID *)&original, fake);
  LONG error = DetourTransactionCommit();
  if (error != NO_ERROR) {
    Log("ERROR", "<DetourTransactionCommit> failed, error = %d", error);
    delete result;
    return nullptr;
  }  

  result->m_original = original;
  result->m_fake = fake;

  return result;
}

void RemoveHook(Hook *hook) {
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  DetourDetach((PVOID *)&hook->m_original, hook->m_fake);
  LONG error = DetourTransactionCommit();
  if (error != NO_ERROR) {
    Log("ERROR", "<DetourTransactionCommit> failed, error = %d", error);
    return;
  }
}