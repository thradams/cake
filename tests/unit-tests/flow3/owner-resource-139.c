
#pragma safety enable

/* p->name never assigned: an uninitialized owner owns nothing, free(p) is clean */

_Uninitialized void * _Owner _Opt malloc(int i);
void free( void * _Owner _Opt  p);

struct X {
  char * _Owner name;
};

int main() {
   struct X * _Owner _Opt p = malloc(sizeof * p);
   free(p);
}
