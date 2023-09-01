void free( void* owner ptr);
void* owner malloc(int size);

void f(int condition) 
{
    int * owner p = malloc(sizeof(int));
    free(p);
    free(p);

    int * owner p2 = p;
    int * owner p3;
    p3 = p;
}
