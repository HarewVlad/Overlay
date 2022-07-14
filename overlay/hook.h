struct Hook {
  void Initialize(PVOID *original, PVOID fake) {
    m_original = original;
    m_fake = fake;
  };

  bool Enable();
  bool Disable();

  PVOID *m_original;
  PVOID m_fake;
};