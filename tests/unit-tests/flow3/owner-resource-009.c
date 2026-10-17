
#pragma safety enable

_Uninitialized void* _Owner _Opt malloc(unsigned long size);

int * _Owner _Opt f(int c) {
    int * _Owner _Opt p = malloc(sizeof *p);
    return p;
}

