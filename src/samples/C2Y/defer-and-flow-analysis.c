#pragma flow enable

void f2(int i){}
void f(int k)
{
  int i;
  _Defer f2(i);
  
  if (k > 1)
   return;
}
