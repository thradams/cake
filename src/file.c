#pragma safety enable


void free(void* _Owner _Opt ptr);

struct X {
  char *_Owner _Opt name;
  int i ;
};

void x_destroy([[dtor]] struct X * x)
{
    x->i = 0;
 // free(x->name);
}
