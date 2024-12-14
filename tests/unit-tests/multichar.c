int main()
{
  static_assert('ab' == 'a' * 256 + 'b');
  static_assert(sizeof(char) == 1);
  static_assert(true == 1);
  static_assert(false == 0);
}
