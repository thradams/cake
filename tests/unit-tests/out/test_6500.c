void free( void* /*_Owner*/ ptr);
void* /*_Owner*/ malloc(int size);

void f(int condition) 
{
    int * /*_Owner*/ p = malloc(sizeof(int));
  
    if (condition)
       goto end;
  end:
    free(p);
}
