int main()
{
    goto a;
    a:;
    a:;
}
#pragma cake diagnostic check "-E1440"

