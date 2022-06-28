Hook *CreateHook(void *original, void *fake) {
  Hook *result = new Hook {};

  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  DetourAttach(&original, fake);
  LONG error = DetourTransactionCommit();
  if (error != NO_ERROR) {
    std::cout << "Create Hook error: " << error;
    assert(0);
  }  

  result->m_original = original;
  result->m_fake = fake;

  return result;
}

void RemoveHook(Hook *hook) {
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  DetourDetach(&hook->m_original, hook->m_fake);
  LONG error = DetourTransactionCommit();
  if (error != NO_ERROR) {
    std::cout << "Remove hook error: " << error;
    assert(0);
  }
}