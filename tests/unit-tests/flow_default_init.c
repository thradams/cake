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
   _Opt struct X x = {0};   
   assert_state(x.text, "null");
   assert_state(x.p1, "null");
   assert_state(x.i, "zero");
   assert_state(x.pY, "null");
}

