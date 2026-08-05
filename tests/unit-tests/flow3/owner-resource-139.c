
#pragma safety enable

/*
   p->name is never assigned before free(p) discards the whole struct
   through void*. An UNINITIALIZED owner owns nothing -- there was never a
   resource assigned into it to leak -- so this stays clean, the same way
   zero and moved already do (see the UNINITIALIZED case in
   flow3_check_object_at_exit's "not moved" check).
*/

void * _Owner _Opt _Uninitialized malloc(int i);
void free( void * _Owner _Opt  p);

struct X {
  char * _Owner name;
};

int main() {
   struct X * _Owner _Opt p = malloc(sizeof * p);
   free(p);
}
