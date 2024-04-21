#pragma nullable enable
#pragma ownership enable

void free(void * _Owner _Opt p);
struct X {
  char * _Owner text; 
};

void x_destroy(_Opt struct X * _Obj_owner x) {
  static_debug(x);
  free(x->text); 
}

void x_delete(_Opt struct X * _Owner _Opt p) { 
  static_debug(p);
  if (p) {
    x_destroy(p); /* *p is moved*/
    free(p);
  }
}

int main() {
   struct X * _Owner pX = calloc(1, sizeof * pX);
   if (pX) {
     /*...*/;
     x_delete( pX); 
   }
 } 
