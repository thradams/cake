/* Cake x86_x64_gcc */

static void _cake_memcpy(void * dest, const void * src, unsigned long n)
{
  char *csrc;
  char *cdest;
  unsigned long i; 

  csrc = (char *)src;
  cdest = (char *)dest;
  for (i = 0; i < n; i++) cdest[i] = csrc[i]; 
}

extern int printf(char * __format, ...);
extern int *wcstok(int * __s, int * __delim, int ** __ptr);
extern int puts(char * __s);

int main(void)
{
    int input[26];
    int * buffer;
    int * token;

    _cake_memcpy(input, L"A bird came down the walk", 26);
    printf("Parsing the input string '%ls'\n", input);
    token = wcstok(input, L" ", &buffer);
    while (token)
    {
        printf("%ls\n", token);
        token = wcstok(0, L" ", &buffer);
    }
    printf("Contents of the input string now: '");
    {
        unsigned int long n;

        n = 0;
        for (; n < 26L; ++n)
        input[n] ? printf("%lc", input[n]) : printf("\\0");
    }
    puts("'");
}


