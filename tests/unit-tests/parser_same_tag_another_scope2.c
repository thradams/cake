/* C23 N3037: enum redefined in an inner scope */

enum E
{
  A
} e1;

int main()
{
  {
    /* same tag, same content: same type */
    enum E
    {
      A
    } e2;
    static_assert(
        _Generic(typeof(e1), typeof(e2): 1, default: 0));
  }

  {
    /* same tag, different content: different type */
    enum E
    {
      B
    } e3;
    static_assert(
        _Generic(typeof(e1), typeof(e3): 0, default: 1));
  }
}
