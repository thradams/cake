
enum E
{
  A
} e1;

int main()
{
  enum E
  {
    A
  } e2;
  static_assert(
      _Generic(typeof(e1), typeof(e2): 0, default: 1));
}