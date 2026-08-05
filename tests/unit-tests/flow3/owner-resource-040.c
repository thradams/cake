#pragma safety enable



void* _Owner _Opt _Uninitialized malloc(unsigned size);
void free(void* _Owner _Opt  ptr);

int main() {
   void * _Owner _Opt p = malloc(1);
   do{
      /* FIXED: used to ALSO warn "object '(*p)' lifetime has ended"
         here -- free's parameter is void*, so it never accesses *p's
         structure; only the "p is moved" warning below is real. */
      free(p); //lint 32 object 'p' is moved (see line 9)
   }
   while(0);   
}
