#pragma safety enable


void free(void * _Owner p);
struct X;
struct X f();
struct X { char * _Owner p; };
int main()
{
    struct X x = 1 ? f() : f(); 
    free(x.p);
}