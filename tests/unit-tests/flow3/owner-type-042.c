#pragma safety enable


struct Y {
  char * _Owner p0;
  int * _Owner p2;
  double i2;
};

struct X {
  char * _Owner text;
  int * _Owner p1;
  int i;
  struct Y  *pY;
};

int main() {   
   _Opt struct X x = {};   
   compile_assert(x.text == 0);
   compile_assert(x.p1 == 0);
   compile_assert(x.i == 0);
   compile_assert(x.pY == 0);
}
