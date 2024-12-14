#pragma safety enable


void f(int * _Owner _Opt p);
int main()
{
   int * _Owner _Opt p = 0;
   p = ((void *) 0); 
   f(0);
   f((void *) 0);
   f(nullptr);
}
