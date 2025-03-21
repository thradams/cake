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

void init(_Ctor struct X * p);
void destroy(_Dtor struct X *  p);

int main() {
   struct X x;
   init(&x);

   static_state(x.p1, "not-null ");
   static_state(x.i, "zero | not-zero");
   static_state(x.pY, "not-null");
   static_state(x.pY->p0, "not-null ");
   static_state(x.pY->p2, "not-null ");
   static_state(x.pY->i2, "zero | not-zero");
   destroy(&x);
}

