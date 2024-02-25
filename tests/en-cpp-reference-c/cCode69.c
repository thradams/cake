//en.cppreference.com/w/c/io/ungetc.html
#include <ctype.h>
#include <stdio.h>
 
void demo_scanf(const char* fmt, FILE* s)
{
    while (*fmt != '\0') {
        if (*fmt == '%') {
            int c;
            switch (*++fmt) {
                case 'u':
                    while (isspace(c=getc(s))) {}
                    unsigned int num = 0;
                    while (isdigit(c)) {
                        num = num*10 + c-'0';
                        c = getc(s);
                    }
                    printf("%%u scanned %u\n", num);
                    ungetc(c, s);
                    break;
                case 'c':
                    c = getc(s);
                    printf("%%c scanned '%c'\n", c);
                    break;
            }
        } else {
            ++fmt;
        }
    }
}
 
int main(void)
{
    FILE* f = fopen("input.txt", "w+");
    if (f != NULL) {
        fputs("123x", f);
        rewind(f); 
        demo_scanf("%u%c", f);
        fclose(f);
    }
    return 0;
}