#pragma safety enable


void F(int i);
_Owner int make();
int main()
{
    F(make()); //lint 23 cannot assign a temporary owner to non-owner object
}
