
void __cdecl exit(int _Code);

void exit_now(int i)
{
    if (i > 0)
    {
        exit(i);
    }
}

int __cdecl puts(char * _Buffer);

int main(void)
{
    puts("Preparing to exit...");
    exit_now(2);
    puts("This code is never executed.");
}

