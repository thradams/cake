
static void _cake_memcpy(void * dest, const void * src, unsigned long n)
{
  char *csrc = (char *)src;
  char *cdest = (char *)dest;
  for (int i=0; i<n; i++) cdest[i] = csrc[i]; 
}

void T6()
{
    char s[4];
    char s2[4];

    _cake_memcpy(s, "123", 4);
    s2 = s;
}

