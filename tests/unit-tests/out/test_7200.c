#pragma safety enable


void free( void * /*_Owner*/ p);

struct X {
  char * /*_Owner*/ name;
};

void x_destroy( struct X * /*_Obj_owner*/ p);

struct Y {
  struct X x;
};

void y_destroy(struct Y * /*_Obj_owner*/ p) {
   x_destroy(&p->x);
}
