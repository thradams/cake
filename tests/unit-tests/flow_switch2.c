#pragma safety enable



void* _Owner _Opt calloc(int n ,unsigned size);

char* _Owner _Opt f(int i)
{
    char* _Owner _Opt p = calloc(1,2);
    switch (i)
    {
        case 1:
            break;
        case 2:
            break;
    }

    return p;
}
