
#pragma flow enable

struct X{
  int i;
};

void f(char *s){
}

void  f2(struct X *p){}
int main(void)
{
  char buffer[20];
  f(buffer);
  struct X x;
  f2(&x);
}