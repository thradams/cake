#pragma safety enable


void* _Owner _Clear calloc(unsigned long n , unsigned long size);
void free(void* _Owner _Opt ptr);

struct Y {
  char * _Owner _Opt p0;
  int * _Owner _Opt p2;
  double i2;
};

struct X {
  char * _Owner _Opt text;
  int * _Owner _Opt p1;
  int i;
  struct Y  * _Opt pY;
};

int main() {   
   struct X * _Owner x = calloc(1,sizeof * x);
 
   compile_assert(x->p1 == 0);
   compile_assert(x->i == 0);
   compile_assert(x->pY == 0);
   free(x);
}
