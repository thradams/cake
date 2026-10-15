#pragma safety enable

void* _Owner _Opt malloc(unsigned long size);
void free(void* _Owner _Opt p);

void while_pointer_condition(void)
{
    char* _Owner _Opt p = malloc(1);
    while (p)
    {
        free(p); //lint 32
    }
}

void while_unrelated_condition(int c)
{
    char* _Owner _Opt p = malloc(1);
    while (c)
    {
        free(p); //lint 32
    }
} //lint 29

void do_while(int c)
{
    char* _Owner _Opt p = malloc(1);
    do
    {
        free(p); //lint 32
    }
    while (c);
}

void if_after_conditional_free(int c)
{
    char* _Owner _Opt p = malloc(1);
    if (c)
    {
        free(p);
    }
    if (p)
    {
        free(p); //lint 32
    }
}

void free_and_advance(int c)
{
    char* _Owner _Opt p = malloc(1);
    while (c)
    {
        free(p);
        p = malloc(1);
    }
    free(p);
}
