int main()
{
   int a = 2 / 0;
#pragma cake diagnostic check "-Wdiv-by-zero"
}
