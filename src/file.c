void free( void* owner ptr);
void* owner malloc(int size);


struct X { 
    char * owner p;
};
void x_destroy(struct X * obj_owner p);
void f(struct X* x)
{
    x_destroy(x);        
}
