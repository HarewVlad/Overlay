void *GetFunctionObfuscated(HMODULE module, const char *name,
                            unsigned long long key) {
  int length = strlen(name);
  char *result = (char *)alloca(length + 1);
  memset(result, 0, length + 1);
  memcpy(result, name, length);

  unsigned int shift = 0;
  for (int i = length - 1; i >= 0; --i) {
    unsigned int offset = (key >> shift) & 0xF;
    result[i] ^= (i ^ 0x20 + offset);

    shift += 4;
    shift %= 64;
  }

  return GetProcAddress(module, result);
}