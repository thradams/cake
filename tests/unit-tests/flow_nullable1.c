#pragma safety enable



int* _Owner make1();
int* _Owner _Opt make2();
void free(void * _Owner _Opt p);


void f(int condition)
{
  int * _Owner _Opt p = 0;
  static_state(p, "null");

  if (condition)
  {
       static_state(p, "null");
       p = make1();
       static_state(p, "not-null ");
       free(p);
       p = make2();
       static_state(p, "null | not-null ");
  }
  else
  {
    static_state(p, "null");
  }
  free(p);
}