/* Cake x86_x64_gcc */
union __tag2 {
    unsigned int __wch;
    char __wchb[4];
};

struct __tag1 {
    int __count;
    union __tag2  __value;
};


static void _cake_memcpy(void * dest, const void * src, unsigned long n)
{
  char *csrc;
  char *cdest;
  unsigned long i; 

  csrc = (char *)src;
  cdest = (char *)dest;
  for (i = 0; i < n; i++) cdest[i] = csrc[i]; 
}

extern char *setlocale(int __category, char * __locale);
extern void *memset(void * __s, int __c, unsigned int long __n);
extern int printf(char * __format, ...);
extern int puts(char * __s);
extern unsigned int long wcrtomb(char * __s, int __wc, struct __tag1 * __ps);

int main(void)
{
    struct __tag1  state;
    int in[5];
    unsigned int long in_sz;
    char out[];
    char * p;
    unsigned int long out_sz;

    setlocale(6, "en_US.utf8");
    memset(&state, 0, 8L);
    _cake_memcpy(in, L"z\xc3\x9f\xe6\xb0\xb4\xf0\x9f\x8d\x8c", 5);
    in_sz = 5L;
    printf("Processing %zu wchar_t units: [ ", in_sz);
    {
        unsigned int long n;

        n = 0;
        for (; n < in_sz; ++n)
        printf("%#x ", (unsigned int)in[n]);
    }
    puts("]");
    p = out;
    {
        unsigned int long n;

        n = 0;
        for (; n < in_sz; ++n)
        {
            int rc;

            rc = wcrtomb(p, in[n], &state);
            if (rc == -1)
            {
                break;
            }
            p += rc;
        }
    }
    out_sz = p - out;
    printf("into %zu UTF-8 code units: [ ", out_sz);
    {
        unsigned int long x;

        x = 0;
        for (; x < out_sz; ++x)
        printf("%#x ", +(unsigned char)out[x]);
    }
    puts("]");
}


