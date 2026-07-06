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
extern int printf(char * __format, ...);
extern int puts(char * __s);
extern void *memset(void * __s, int __c, unsigned int long __n);
extern unsigned int long mbrtoc32(unsigned int * __pc32, char * __s, unsigned int long __n, struct __tag1 * __p);

int main(void)
{
    char in[11];
    unsigned int long in_size;
    unsigned int out[11];
    unsigned int * p_out;
    char * p_in;
    char * end;
    struct __tag1  state;
    unsigned int long rc;
    unsigned int long out_size;

    setlocale(6, "en_US.utf8");
    _cake_memcpy(in, "z\xc3\x9f\xe6\xb0\xb4\xf0\x9f\x8d\x8c", 11);
    in_size = 11L;
    printf("Processing %zu UTF-8 code units: [ ", 11L);
    {
        unsigned int long i;

        i = 0;
        for (; i < 11L; ++i)
        printf("0x%02x ", (unsigned char)in[i]);
    }
    puts("]");
    p_out = out;
    p_in = in;
    end = in + 11L;
    memset(&state, 0, 8L);
    while ((rc = mbrtoc32(p_out, p_in, end - p_in, &state)))
    {
        ;
        if (rc == 18446744073709551615L)
        {
            break;
        }
        if (rc == 18446744073709551614L)
        {
            break;
        }
        p_in += rc;
        ++p_out;
    }
    out_size = p_out + 1 - out;
    printf("into %zu UTF-32 code units: [ ", out_size);
    {
        unsigned int long i;

        i = 0;
        for (; i < out_size; ++i)
        printf("0x%08X ", out[i]);
    }
    puts("]");
}


