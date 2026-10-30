#pragma safety enable



_Clear void * _Owner calloc(int n , int i);

char* _Owner f(int i)
{
    char* _Owner p = calloc(1,2);        
    switch (i) {
        case 1: break;
    }
    return p;
}
