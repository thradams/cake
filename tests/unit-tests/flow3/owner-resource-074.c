#pragma safety enable

/* x is _Uninitialized and its owner members never assigned: an uninitialized owner owns nothing, free(x) is clean */

_Uninitialized void* _Nonnull _Owner malloc(unsigned long size);
void free(void* _Owner _Opt ptr);

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
   struct X * _Owner x = malloc(sizeof * x);

  // assert_state(x->p1, "uninitialized");
   //assert_state(x->i, "uninitialized");
   //assert_state(x->pY, "uninitialized");
   free(x);
}
