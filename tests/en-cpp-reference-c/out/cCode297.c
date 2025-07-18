
int __cdecl puts(char * _Buffer);

void f1(void)
{
    puts("f1");
}

void f2(void)
{
    puts("f2");
}

int __cdecl atexit(void (__cdecl *)(void));

int main(void)
{
    if (!atexit(f1) && !atexit(f2) && !atexit(f2))
    {
        return 0;
    }
    return 1;
}

