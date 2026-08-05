#pragma safety enable


void free( void * _Owner p);

struct X {
  char * _Owner name;
};

void x_destroy( _Dtor struct X *  p);

struct Y {
  struct X x;
};

void y_destroy(_Dtor struct Y *  p) {
   x_destroy(&p->x);
}
