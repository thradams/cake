#pragma safety enable



struct X {
  void * /*_Owner*/ /*_Opt*/ text;
};

void f(const struct X* list);

int main()
{
  struct X x = {0};
  f(&x);
}
