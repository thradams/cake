#pragma safety enable


void free( void * /*_Owner*/ p);
struct X {
  char * /*_Owner*/ text;
};
void x_delete( struct X * /*_Owner*/ /*_Opt*/ p)
{
    if (p)
    {
      free(p->text);
      free(p);
    }
}
