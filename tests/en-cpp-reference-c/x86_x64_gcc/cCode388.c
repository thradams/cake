/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern unsigned int towupper(unsigned int __wc);
extern char *setlocale(int __category, char * __locale);

int main(void)
{
    int wc;

    wc = 383;
    printf("in the default locale, towupper(%#x) = %#x\n", wc, towupper(wc));
    setlocale(6, "en_US.utf8");
    printf("in Unicode locale, towupper(%#x) = %#x\n", wc, towupper(wc));
}


