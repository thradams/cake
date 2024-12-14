
enum E 
{
  A
};

int main()
{
  enum E
  {
    B
  } e2;

  static_assert(
      _Generic(e2, enum E: 1, default: 0));
}
