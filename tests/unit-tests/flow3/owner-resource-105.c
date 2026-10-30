#pragma safety enable


_Uninitialized void* _Owner malloc(unsigned long size);
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
