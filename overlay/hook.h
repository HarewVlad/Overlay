struct Hook {
  PVOID *m_original;
  PVOID m_fake;
};

Hook *CreateHook(PVOID *original, PVOID *fake);
bool EnableHook(Hook *hook);
bool RemoveHook(Hook *hook);