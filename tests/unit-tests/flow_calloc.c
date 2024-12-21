#pragma safety enable


void* _Owner calloc(unsigned long n , unsigned long size);
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
   static_state(x, "null | not-null ");

   static_state(x->p1, "null ");
   static_state(x->i, "zero");
   static_state(x->pY, "null");
   static_state(x->pY->p0, "");
   static_state(x->pY->p2, "");
   static_state(x->pY->i2, "");   
   free(x);
}

