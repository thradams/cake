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
extern unsigned int long mbsrtowcs(int * __dst, char ** __src, unsigned int long __len, struct __tag1 * __ps);
extern int wprintf(int * __format, ...);

void print_as_wide(char * mbstr)
{
    struct __tag1  state;
    unsigned int long len;
    int wstr[];

    memset(&state, 0, 8L);
    len = 1 + mbsrtowcs(0, &mbstr, 0, &state);
    mbsrtowcs(&wstr[0], &mbstr, len, &state);
    wprintf(L"Wide string: %ls \n", wstr);
    wprintf(L"The length, including L'\\0': %zu\n", len);
}


extern char *setlocale(int __category, char * __locale);

int main(void)
{
    setlocale(6, "en_US.utf8");
    print_as_wide("z\u00df\u6c34\U0001f34c");
}


