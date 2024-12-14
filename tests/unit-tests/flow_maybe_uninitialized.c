#pragma safety enable

int printf(const char* restrict format, ...);

struct X {
    int i;
} ;

void f(_Out struct X * x, int i)
{
   if (i > 0){
     x->i =1;
   }
} 
#pragma cake diagnostic check "-Wanalyzer-maybe-uninitialized"

//external variables, in this case object *x cannot leave scope
//with uninitialized values.

int main(){  
  struct X x;
  f(&x, 1);
  printf("%d", x.i);
}
 