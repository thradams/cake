#pragma safety enable



int* _Owner make1();
int* _Owner _Opt make2();
void free(void * _Owner _Opt p);


void f(int condition)
{
  int * _Owner _Opt p = 0;
  assert_state(p, "null");

  if (condition)
  {
       assert_state(p, "null");
       p = make1();
       assert_state(p, "not-null ");
       free(p);
       p = make2();
       assert_state(p, "null | not-null ");
  }
  else
  {
    assert_state(p, "null");
  }
  free(p);
}