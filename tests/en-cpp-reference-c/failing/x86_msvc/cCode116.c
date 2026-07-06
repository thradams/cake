/* Cake x86_msvc */

int __cdecl puts(char * _Buffer);

void g(int *)
{
    puts("Function g called");
}


int main()
{
    nullptr_t cloned_nullptr;
    void * cloned_NULL;
    int cloned_zero;

    g(0);
    g(0);
    g(0);
    cloned_nullptr = 0;
    g(cloned_nullptr);
    cloned_NULL = 0;
    cloned_zero = 0;
    puts("void*");
    puts("integer");
    puts("nullptr_t");
    puts("void*");
}


