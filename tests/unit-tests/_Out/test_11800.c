#pragma safety enable


struct Y {
  char * /*_Owner*/ p0;
  int * /*_Owner*/ p2;
  double i2;
};

struct X {
  char * /*_Owner*/ text;
  int * /*_Owner*/ p1;
  int i;
  struct Y  *pY;
};

void init(/*_Out*/ struct X * p);
void destroy(/*_Out*/ struct X * /*_Obj_owner*/ p);

int main() {
   struct X x;
   init(&x);

   
   
   
   
   
   
   destroy(&x);
}

