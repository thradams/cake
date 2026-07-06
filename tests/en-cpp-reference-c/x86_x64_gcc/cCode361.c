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

struct __tag1  state;
extern char *setlocale(int __category, char * __locale);
extern int printf(char * __format, ...);
extern int puts(char * __s);
extern unsigned int long mbrtoc16(unsigned int short * __pc16, char * __s, unsigned int long __n, struct __tag1 * __p);

int main(void)
{
    char in[11];
    unsigned int long in_sz;
    unsigned int short out[11];
    char * p_in;
    char * end;
    unsigned int short * p_out;
    unsigned int long out_sz;

    setlocale(6, "en_US.utf8");
    _cake_memcpy(in, "z\xc3\x9f\xe6\xb0\xb4\xf0\x9f\x8d\x8c", 11);
    in_sz = 11L;
    printf("Processing %zu UTF-8 code units: [ ", 11L);
    {
        unsigned int long n;

        n = 0;
        for (; n < 11L; ++n)
        printf("%#x ", (unsigned char)in[n]);
    }
    puts("]");
    p_in = in;
    end = in + 11L;
    p_out = out;
    {
        unsigned int long rc;

        for (; (rc = mbrtoc16(p_out, p_in, end - p_in, &state)); )
        {
            if (rc == 18446744073709551615L)
            {
                break;
            }
            else
            {
                if (rc == 18446744073709551614L)
                {
                    break;
                }
                else
                {
                    if (rc == 18446744073709551613L)
                    {
                        p_out += 1;
                    }
                    else
                    {
                        p_in += rc;
                        p_out += 1;
                    }
                }
            }
            ;
        }
    }
    out_sz = p_out - out + 1;
    printf("into %zu UTF-16 code units: [ ", out_sz);
    {
        unsigned int long x;

        x = 0;
        for (; x < out_sz; ++x)
        printf("%#x ", out[x]);
    }
    puts("]");
}


