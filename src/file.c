#pragma nullable enable
#pragma ownership enable

struct X {
  void * _Owner _Opt text;
};
void x_destroy(struct X x);

struct X f();

int main()
{
  struct X x = f();
  _View struct X x2 = x;
  

}
