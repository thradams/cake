#pragma nullable enable
#pragma ownership enable

void free( void* /*_Owner*/ /*_Opt*/ ptr);
void* /*_Owner*/ /*_Opt*/ malloc(int size);

void f(int condition) 
{
    int * /*_Owner*/ /*_Opt*/ p = malloc(sizeof(int));
  
    if (condition)
       goto end;
  end:
    free(p);
}
