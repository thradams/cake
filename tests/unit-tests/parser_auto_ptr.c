int main()
{

    int x = 1;
    auto* p = &x;
#pragma cake diagnostic check "-E1290"
    * p = 0;
}

