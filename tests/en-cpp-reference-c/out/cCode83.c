
int __cdecl puts(char * _Buffer);
int __cdecl putchar(int _Character);

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

