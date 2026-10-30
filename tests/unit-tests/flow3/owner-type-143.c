#pragma safety enable



_Clear void* _Owner calloc(int n ,unsigned size);

char* _Owner f(int i)
{
    char* _Owner p = calloc(1,2);
    switch (i)
    {
        case 1:
            break;
        case 2:
            break;
    }

    return p;
}
