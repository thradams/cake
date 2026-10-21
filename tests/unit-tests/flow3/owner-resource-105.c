#pragma safety enable


_Uninitialized void* _Owner _Opt malloc(unsigned long size);
void free(void* _Owner ptr);

void f5()
{
    void * _Owner _Opt p = malloc(1);
    
    if (p) {
       free(p);
       return;
    }
    
    compile_assert(p == 0); 
}
