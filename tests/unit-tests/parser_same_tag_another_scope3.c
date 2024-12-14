
enum E
{
  A
};
int main()
{
  enum E
  {
    B
  } e2 = 0;
  enum E e1 = 0;

  static_assert(
      _Generic(typeof(e2), typeof(e1): 1, default: 0));
}
