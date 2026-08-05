#pragma safety enable


void* _Owner _Uninitialized malloc(unsigned size);
void free(void* _Owner ptr);


struct X {  char *_Owner name; };
struct Y { struct X x; };

void f(struct Y * y, _Dtor struct X *  p) 
{
    free(y->x.name);
    y->x = *p;
}
