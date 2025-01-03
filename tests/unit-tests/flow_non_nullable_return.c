#pragma safety enable


int * f();
int main()
{
  int * _Opt p = f();
  if (p)
    return 0;
#pragma cake diagnostic check "-Wflow-not-null"  
}
