/* Cake x64_msvc */

static void memcpy2(void * dest, const void * src, unsigned long long n);

int __cdecl puts(char * _Buffer);
void *__cdecl memset(void * _Dst, int _Val, unsigned long long _Size);

int main(void)
{
    char str[23];

    memcpy2(str, "ghghghghghghghghghghgh", 23);
    puts(str);
    memset(str, 97, 5);
    puts(str);
}

static void memcpy2(void * dest, const void * src, unsigned long long n)
{
    char *csrc;
    char *cdest;
    unsigned long long i; 

    csrc = (char *)src;
    cdest = (char *)dest;
    for (i = 0; i < n; i++)
    {
       cdest[i] = csrc[i]; 
    }
}

