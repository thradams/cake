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

void init(_Out struct X * p);
void destroy(_Dtor struct X *  p);

int main() {
   struct X x;
   init(&x);

   /* FIXED: the _Out call keeps non-_Opt pointer/_Owner members non-null, so the compile_asserts hold and destroy(&x) does not warn */
   compile_assert(x.p1 != 0);
   //assert_state(x.i, "zero | not-zero");
   compile_assert(x.pY != 0);
   compile_assert(x.pY->p0 != 0);
   compile_assert(x.pY->p2 != 0);
   //assert_state(x.pY->i2, "zero | not-zero");
   destroy(&x);
}
