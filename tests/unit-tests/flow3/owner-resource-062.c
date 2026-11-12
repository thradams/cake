#pragma safety enable



void free(void* _Owner _Opt ptr);
_Uninitialized void* _Owner malloc(int size);

void f(int c)
{
    int* _Owner p = malloc(sizeof(int)); // owner object (p) not moved (var-false see line 10)
    if (c)
    {
        free(p);
    }
} //lint 29 owner object (p) not moved (var-false see line 10)
