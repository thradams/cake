int main()
{
    _Generic(1, double: 11, int : (typeof(1)*)0);
}