#pragma safety enable



int* _Nonnull _Owner make1();
int* _Owner make2();
void free(void * _Owner _Opt p);


void f(int condition)
{
  int * _Owner p = 0;
  compile_assert(p == 0);

  if (condition)
  {
       compile_assert(p == 0);
       p = make1();
       compile_assert(p != 0);
       free(p);
       p = make2();
       //assert_state(p, "null | not-null ");
  }
  else
  {
    compile_assert(p == 0);
  }
  free(p);
}
