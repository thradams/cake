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
extern int sscanf(char * __s, char * __format, ...);
extern int printf(char * __format, ...);

int main(void)
{
    int i;
    int j;
    float x;
    float y;
    char str1[10];
    char str2[4];
    int warr[2];
    char input[40];
    int ret;

    setlocale(6, "en_US.utf8");
    _cake_memcpy(input, "25 54.32E-1 Thompson 56789 0123 56\xc3\x9f\xe6\xb0\xb4", 40);
    ret = sscanf(input, "%d%f%9s%2d%f%*d %3[0-9]%2lc", &i, &x, str1, &j, &y, str2, warr);
    printf("Converted %d fields:\ni = %d\nx = %f\nstr1 = %s\nj = %d\ny = %f\nstr2 = %s\nwarr[0] = U+%x\nwarr[1] = U+%x\n", ret, i, x, str1, j, y, str2, warr[0], warr[1]);
}


