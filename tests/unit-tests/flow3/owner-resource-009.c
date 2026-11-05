
#pragma safety enable

_Uninitialized void* _Owner malloc(unsigned long size);

int * _Owner f(int c) {
    int * _Owner p = malloc(sizeof *p);
    if (p) { *p = 1; }
    return p;
}

