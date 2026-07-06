/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern unsigned int towlower(unsigned int __wc);
extern char *setlocale(int __category, char * __locale);

int main(void)
{
    int wc;

    wc = 400;
    printf("in the default locale, towlower(%#x) = %#x\n", wc, towlower(wc));
    setlocale(6, "en_US.utf8");
    printf("in Unicode locale, towlower(%#x) = %#x\n", wc, towlower(wc));
}


