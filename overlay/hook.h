struct Hook {
  void *m_original;
  void *m_fake;
};

Hook *CreateHook(void *original, void *fake);
void RemoveHook(Hook *hook);