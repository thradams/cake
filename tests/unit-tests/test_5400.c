#pragma safety enable


struct X {
  char * _Owner name;
};

_View struct X f();

int main()
{
  f();
}
