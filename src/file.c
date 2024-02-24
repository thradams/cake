
void F(int i);
_Owner int make();
int main()
{ int a1=1/0;
    F(make());
  
#pragma cake diagnostic check "-Wtemp-owner"

 
}
