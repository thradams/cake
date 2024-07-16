#pragma safety enable


void F(int i);
/*_Owner*/ int make();
int main()
{
    F(make());
}
#pragma cake diagnostic check "-Wtemp-owner"

