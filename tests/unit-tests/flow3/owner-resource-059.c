#pragma safety enable

/* `if (p)` on a non-_Opt p keeps a path where the body never ran and .name stays _Uninitialized; an uninitialized owner owns nothing, so free(p) is not a leak */

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
