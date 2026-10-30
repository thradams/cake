#pragma safety enable



struct X {
  void * _Owner text;
};

void f(const struct X* list);

int main()
{
  struct X x = {};
  f(&x);
}
