#pragma safety enable


void free( void * _Owner p);
struct X {
  char * _Owner text;
};
void x_delete( struct X * _Owner _Opt p)
{
    if (p)
    {
      free(p->text);
      /* FIXED: this used to ALSO warn about '(*p).text' (null-pointer
         and moved-object checks) here -- free's own parameter is void*,
         so it never accesses *p's structure at all; passing p to it is
         just consuming p itself. */
      free(p);
    }
}
