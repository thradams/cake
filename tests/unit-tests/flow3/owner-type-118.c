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

   /* FIXED (this whole block used to fail): init's _Out call
      invalidated every member of x to a generic ANY value (could be
      zero), even though p1, pY, text etc. are all plain (non-_Opt)
      pointer/_Owner types whose own type already guarantees they're
      never null. That made compile_assert(x.p1 != 0) and
      compile_assert(x.pY != 0) below fail with "could be any value,
      including zero", and destroy(&x) at the end warn "passing a
      possible null pointer" for text/p1/pY -- none of it provable false
      or a real bug, just a lost non-null guarantee. */
   compile_assert(x.p1 != 0);
   //assert_state(x.i, "zero | not-zero");
   compile_assert(x.pY != 0);
   compile_assert(x.pY->p0 != 0);
   compile_assert(x.pY->p2 != 0);
   //assert_state(x.pY->i2, "zero | not-zero");
   destroy(&x);
}
