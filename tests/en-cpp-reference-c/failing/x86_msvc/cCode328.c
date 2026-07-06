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

char *__cdecl strcat(char * _Destination, char * _Source);
int __cdecl puts(char * _Buffer);

int main(void)
{
    char str[50];
    char str2[50];

    _cake_memcpy(str, "Hello ", 50);
    _cake_memcpy(str2, "World!", 50);
    strcat(str, str2);
    strcat(str, " ...");
    strcat(str, " Goodbye World!");
    puts(str);
}


