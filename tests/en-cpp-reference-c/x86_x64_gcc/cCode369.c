/* Cake x86_x64_gcc */
union __tag2 {
    unsigned int __wch;
    char __wchb[4];
};

struct __tag1 {
    int __count;
    union __tag2  __value;
};


extern void *memset(void * __s, int __c, unsigned int long __n);
extern unsigned int long wcsrtombs(char * __dst, int ** __src, unsigned int long __len, struct __tag1 * __ps);
extern int printf(char * __format, ...);

void print_wide(int * wstr)
{
    struct __tag1  state;
    unsigned int long len;
    char mbstr[];

    memset(&state, 0, 8L);
    len = 1 + wcsrtombs(0, &wstr, 0, &state);
    wcsrtombs(mbstr, &wstr, len, &state);
    printf("Multibyte string: %s\n", mbstr);
    printf("Length, including '\\0': %zu\n", len);
}


extern char *setlocale(int __category, char * __locale);

int main(void)
{
    setlocale(6, "en_US.utf8");
    print_wide(L"z\u00df\u6c34\U0001f34c");
}


