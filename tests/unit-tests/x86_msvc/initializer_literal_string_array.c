/* Cake x86_msvc */

static void _cake_memcpy(void * dest, const void * src, unsigned int n)
{
  char *csrc;
  char *cdest;
  unsigned int i; 

  csrc = (char *)src;
  cdest = (char *)dest;
  for (i = 0; i < n; i++) cdest[i] = csrc[i]; 
}

void T6()
{
    char s[4];
    char s2[4];

    _cake_memcpy(s, "123", 4);
    s2 = s;
}


