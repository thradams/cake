#pragma safety enable

// the defers of a block run at each exit (return, break, end of block);
// an end of block that is never reached does not run them again.

void* _Owner _Opt malloc(unsigned long n);
void free(void* _Owner _Opt p);

int f(int c)
{
    int* _Owner _Opt p = malloc(sizeof(int));
    defer free(p); // runs at 'return 1' and 'return 0', not at the end of the function

    if (c)
        return 1;

    return 0;
}

int g(int c)
{
    int* _Owner _Opt p = malloc(sizeof(int));
    defer
    {
        free(p); // no "unreachable code", no "'p' was moved"
    }

    if (c)
        return 1;
    return 0;
}
