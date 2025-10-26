/* Cake 0.12.26 x86_msvc */

int __cdecl puts(char * _Buffer);
void __cdecl perror(char * _ErrorMessage);

int main(void)
{
    int rc;

    rc = puts("Hello World");
    if (rc == -1)
    {
        perror("puts()");
    }
}


