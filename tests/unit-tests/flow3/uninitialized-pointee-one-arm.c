#pragma safety enable

_Uninitialized void* _Owner malloc(unsigned int size);
void free(void* _Owner _Nullable p);
void use(int* p);

int* _Owner return_one_arm(int c)
{
    int* _Owner p = malloc(sizeof(int));
    if (p == 0) { return 0; }
    if (c) { *p = 1; }
    return p; //lint 30
}

void argument_one_arm(int c)
{
    int* _Owner p = malloc(sizeof(int));
    if (p == 0) { return; }
    if (c) { *p = 1; }
    use(p); //lint 30
    free(p);
}

int* _Owner return_both_arms(int c)
{
    int* _Owner p = malloc(sizeof(int));
    if (p == 0) { return 0; }
    if (c) { *p = 1; } else { *p = 2; }
    return p;
}

/* each return sees only its own arm */
int* _Owner return_inside_arm(int c)
{
    int* _Owner p = malloc(sizeof(int));
    if (p == 0) { return 0; }
    if (c)
    {
        *p = 1;
        return p;
    }
    *p = 2;
    return p;
}
