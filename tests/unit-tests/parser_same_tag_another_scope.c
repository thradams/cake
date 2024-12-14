
enum E
{
  A
} e1;

int main()
{
  enum E e2;
  static_assert(
      _Generic(typeof(e1), typeof(e2): 1, default: 0));
}