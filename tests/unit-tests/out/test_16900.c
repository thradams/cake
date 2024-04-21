#pragma nullable enable
#pragma ownership enable

int * f();
int main()
{
  int * _Opt p = f();
  if (p)
    return;
#pragma cake diagnostic check "Wnonnull"  
}