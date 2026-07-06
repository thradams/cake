/* Cake x86_x64_gcc */

extern int *wcscpy(int * __dest, int * __src);
extern char *setlocale(int __category, char * __locale);
extern int printf(char * __format, ...);

int main(void)
{
    int * src;
    int dst[];

    src = L"\xe7\x8a\xac means dog";
    wcscpy(dst, src);
    dst[0] = 29399;
    setlocale(6, "en_US.utf8");
    printf("src = %ls\ndst = %ls\n", src, dst);
}


