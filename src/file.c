void* _Owner malloc(unsigned long size);
void free(void* _Owner ptr);

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

struct X f();

int main() {
   struct X x;
   x = f();

   static_state(x.p1, "maybe-null");
   static_state(x.i, "any");
   static_state(x.pY, "maybe-null");
   static_state(x.pY->p0, "maybe-null");
   static_state(x.pY->p2, "maybe-null");
   static_state(x.pY->i2, "any");
   free(x);
}
