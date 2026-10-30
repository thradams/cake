#pragma safety enable


struct Y {
  char * _Nonnull _Owner p0;
  int * _Nonnull _Owner p2;
  double i2;
};

struct X {
  char * _Nonnull _Owner text;
  int * _Nonnull _Owner p1;
  int i;
  struct Y  * _Nonnull pY;
};

int main() {   
   _Opt struct X x = {0};   
   compile_assert(x.text == 0);
   compile_assert(x.p1 == 0);
   compile_assert(x.i == 0);
   compile_assert(x.pY == 0);
}
