
#pragma safety enable

void* _Owner _Opt calloc(unsigned long long n, unsigned long size);
void free(void* _Owner _Opt ptr);

struct  Y { int i ; };
struct  X { int i ; struct Y* _Owner _Opt pY;};

void f(struct X *pX) 
{
  pX->pY = nullptr;  
}

int main(){
  struct X x = {0};
  x.pY = calloc(1, sizeof (struct Y));

  struct X x2 = {0};
  x = x2;

  
}
