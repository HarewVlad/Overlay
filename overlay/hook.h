struct Hook {
  PVOID *m_original;
  PVOID m_fake;
};

Hook *CreateHook(PVOID *original, PVOID fake);
void RemoveHook(Hook *hook);