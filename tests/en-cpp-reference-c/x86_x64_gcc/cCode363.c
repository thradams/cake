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
extern unsigned int long mbrtowc(int * __pwc, char * __s, unsigned int long __n, struct __tag1 * __p);

int main(void)
{
    struct __tag1  state;
    char in[5];
    unsigned int long in_sz;
    int out[];
    char * p_in;
    char * end;
    int * p_out;
    int rc;
    unsigned int long out_sz;

    setlocale(6, "en_US.utf8");
    memset(&state, 0, 8L);
    _cake_memcpy(in, "z\u00df\u6c34\U0001F34C", 5);
    in_sz = 5L;
    printf("Processing %zu UTF-8 code units: [ ", in_sz);
    {
        unsigned int long n;

        n = 0;
        for (; n < in_sz; ++n)
        printf("%#x ", (unsigned char)in[n]);
    }
    puts("]");
    p_in = in;
    end = in + in_sz;
    p_out = out;
    while ((rc = mbrtowc(p_out, p_in, end - p_in, &state)) > 0)
    {
        p_in += rc;
        p_out += 1;
    }
    out_sz = p_out - out + 1;
    printf("into %zu wchar_t units: [ ", out_sz);
    {
        unsigned int long x;

        x = 0;
        for (; x < out_sz; ++x)
        printf("%#x ", out[x]);
    }
    puts("]");
}


