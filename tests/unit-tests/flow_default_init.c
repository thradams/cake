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
   static_state(x.text, "null");
   static_state(x.p1, "null");
   static_state(x.i, "zero");
   static_state(x.pY, "null");
}

