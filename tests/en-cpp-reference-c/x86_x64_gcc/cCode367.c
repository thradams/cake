/* Cake x86_x64_gcc */

extern int mbtowc(int * __pwc, char * __s, unsigned int long __n);
extern unsigned int long strlen(char * __s);
extern int wprintf(int * __format, ...);

void print_mb(char * ptr)
{
    char * end;
    int ret;

    mbtowc(0, 0, 0);
    end = ptr + strlen(ptr);
    ret = 0;
    {
        int wc;

        for (; (ret = mbtowc(&wc, ptr, end - ptr)) > 0; ptr += ret)
        wprintf(L"%lc", wc);
    }
    wprintf(L"\n");
}


extern char *setlocale(int __category, char * __locale);

int main(void)
{
    setlocale(6, "en_US.utf8");
    print_mb("z\u00df\u6c34\U0001F34C");
}


