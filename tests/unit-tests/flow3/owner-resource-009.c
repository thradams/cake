
#pragma safety enable

void* _Owner _Opt _Uninitialized malloc(unsigned long size);

int * _Owner _Opt f(int c) {
    int * _Owner _Opt p = malloc(sizeof *p);
    return p;
}

