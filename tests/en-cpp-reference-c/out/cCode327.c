
static void _cake_memcpy(void * dest, const void * src, unsigned long n)
{
  char *csrc = (char *)src;
  char *cdest = (char *)dest;
  for (int i=0; i<n; i++) cdest[i] = csrc[i]; 
}

int __cdecl puts(char * _Buffer);
void *__cdecl memset(void * _Dst, int _Val, unsigned int _Size);

int main(void)
{
    char str[23];

    _cake_memcpy(str, "ghghghghghghghghghghgh", 23);
    puts(str);
    memset(str, 97, 5);
    puts(str);
}

