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
      /* FIXED: no extra warnings about (*p).text, free takes void* and only consumes p */
      free(p);
    }
}
