#pragma safety enable


void* _Owner _Uninitialized malloc(unsigned long size);
void free(void* _Owner ptr);

void f5()
{
    void * _Owner p = malloc(1);
    
    if (p) {
       free(p);
       return;
    }
    
    compile_assert(p == 0);
}
