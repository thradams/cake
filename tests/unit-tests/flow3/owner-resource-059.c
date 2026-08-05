#pragma safety enable

/*
   p is non-_Opt, so `if (p)` is dead-code defensiveness: p can never
   actually be null. flow3 does not exploit that guarantee to prune the
   false arm, so it still models a path where the if-body (which sets and
   frees p->name) never ran -- on that arm .name is left exactly as
   malloc's _Uninitialized contract leaves it, never given a value. That
   is fine: an UNINITIALIZED owner owns nothing, so free(p) discarding
   *p through void* on that arm is not a leak, the same way zero and
   moved already are not.
*/

void * _Owner _Uninitialized malloc(int i);
void free( void * _Owner _Opt p);

struct X {
  char * _Owner name;
};

int main() {
   struct X * _Owner p = malloc(sizeof * p);
   if (p) {
     p->name = malloc(10);
     free(p->name);
   }
   free(p);
}
