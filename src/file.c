struct X 
{
    int * i;
};

int main()
{
  struct X x;
  static_debug(x.i);
}