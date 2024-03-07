struct X {int i;};
int main(){
  struct X x = {0};
  struct X * p ;
  p = &x;
  p->i = 2;
  static_debug(x);
}