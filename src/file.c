

void* _Owner malloc(unsigned long size);
void free(void* _Owner ptr);

int condition;
void f(void **p);
void f5(int condition)
{
    void * _Owner p = malloc(1);

    //static_debug(p);
    while (p) {
       //static_debug(p);
       if (condition) break;
       //f(&p);
       //return;
    }
    static_debug(p);    
}