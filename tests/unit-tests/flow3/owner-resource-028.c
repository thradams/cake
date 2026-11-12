#pragma safety enable


_Clear void* _Nonnull _Owner calloc(unsigned long n , unsigned long size);
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
  struct Y  * pY;
};

int main() {   
   struct X * _Owner x = calloc(1,sizeof * x);
 
   compile_assert(x->p1 == 0);
   compile_assert(x->i == 0);
   compile_assert(x->pY == 0);
   free(x);
}
