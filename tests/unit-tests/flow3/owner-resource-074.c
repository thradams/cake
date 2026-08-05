#pragma safety enable

/*
   x is allocated _Uninitialized and none of its owner members (.text, .p1)
   are ever assigned before free(x) discards the whole struct through
   void*. An UNINITIALIZED owner owns nothing -- there was never a
   resource assigned into it to leak -- so this stays clean, the same way
   zero and moved already do (see the UNINITIALIZED case in
   flow3_check_object_at_exit's "not moved" check).
*/

void* _Owner _Uninitialized malloc(unsigned long size);
void free(void* _Owner _Opt ptr);

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
   struct X * _Owner x = malloc(sizeof * x);

  // assert_state(x->p1, "uninitialized");
   //assert_state(x->i, "uninitialized");
   //assert_state(x->pY, "uninitialized");
   free(x);
}
