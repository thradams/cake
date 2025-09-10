#pragma safety enable


void F(int i);
_Owner int make();
int main()
{
    //cannot assign a temporary owner to non-owner object
    [[cake::w23]]
    F(make());
}
