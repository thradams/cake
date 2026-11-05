#pragma safety enable

/* issue #531: a pointer to uninitialized storage is returned only if the return type says _Uninitialized */

_Uninitialized void* _Owner malloc(unsigned int size);

int* _Owner direct(void)
{
    return malloc(sizeof(int)); //lint 30
}

int* _Owner through_local(void)
{
    int* _Owner p = malloc(sizeof(int));
    return p; //lint 30
}

int* _Owner initialized(void)
{
    int* _Owner p = malloc(sizeof(int));
    if (p) { *p = 1; }
    return p;
}

int* _Owner one_arm_initialized(int c)
{
    int* _Owner p = malloc(sizeof(int));
    if (c && p)
    {
        *p = 1;
    }
    return p; //lint 30
}

_Uninitialized int* _Owner annotated(void)
{
    return malloc(sizeof(int));
}

void* _Owner untyped(void)
{
    return malloc(1);
}
