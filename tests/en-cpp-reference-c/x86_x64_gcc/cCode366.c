/* Cake x86_x64_gcc */

extern char *setlocale(int __category, char * __locale);
extern unsigned int long mbstowcs(int * __pwcs, char * __s, unsigned int long __n);
extern int wprintf(int * __format, ...);

int main(void)
{
    char * mbstr;
    int wstr[5];

    setlocale(6, "en_US.utf8");
    mbstr = "z\u00df\u6c34\U0001F34C";
    mbstowcs(wstr, mbstr, 5);
    wprintf(L"MB string: %s\n", mbstr);
    wprintf(L"Wide string: %ls\n", wstr);
}


