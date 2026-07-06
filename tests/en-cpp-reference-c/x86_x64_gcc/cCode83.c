/* Cake x86_x64_gcc */

extern int puts(char * __s);
extern int putchar(int __c);

int main(void)
{
    puts("Printable ASCII:");
    {
        int i;

        i = 32;
        for (; i < 127; ++i)
        {
            putchar(i);
            putchar(i % 16 == 15 ? 10 : 32);
        }
    }
}


