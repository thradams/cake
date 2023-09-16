void* owner malloc(unsigned long size);
void free(void* owner ptr);

void f5()
{
    void * owner p = malloc(1);
    
    if (p) {
       free(p);
       return;
    }
    
    static_state(p, "null");    
}
