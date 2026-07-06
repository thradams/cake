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

extern int *wcschr(int * __wcs, int __wc);
extern char *setlocale(int __category, char * __locale);
extern int printf(char * __format, ...);
extern int puts(char * __s);

int main(void)
{
    int arr[12];
    int * cat;
    int * dog;

    _cake_memcpy(arr, L"\xe7\x99\xbd\xe7\x8c\xab \xe9\xbb\x92\xe7\x8c\xab \xd0\xba\xd0\xbe\xd1\x88\xd0\xba\xd0\xb8", 12);
    cat = wcschr(arr, 29483);
    dog = wcschr(arr, 29356);
    setlocale(6, "en_US.utf8");
    if (cat)
    {
        printf("The character \xe7\x8c\xab found at position %td\n", cat - arr);
    }
    else
    {
        puts("The character \xe7\x8c\xab not found");
    }
    if (dog)
    {
        printf("The character \xe7\x8a\xac found at position %td\n", dog - arr);
    }
    else
    {
        puts("The character \xe7\x8a\xac not found");
    }
}


