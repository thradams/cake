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

extern char *setlocale(int __category, char * __locale);
extern unsigned int towctrans(unsigned int __wc, signed int * __desc);
extern signed int *wctrans(char * __property);
extern int printf(char * __format, ...);

int main(void)
{
    int kana[5];
    unsigned int long sz;
    int hira[];

    setlocale(6, "ja_JP.UTF-8");
    _cake_memcpy(kana, L"\xe3\x83\x92\xe3\x83\xa9\xe3\x82\xac\xe3\x83\x8a", 5);
    sz = 5L;
    {
        unsigned int long n;

        n = 0;
        for (; n < sz; ++n)
        hira[n] = towctrans(kana[n], wctrans("tojhira"));
    }
    printf("katakana characters %ls are %ls in hiragana\n", kana, hira);
}


