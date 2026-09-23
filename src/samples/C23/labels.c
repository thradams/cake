/* https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2508.pdf */
int main()
{
  goto label;

  label:
  int x;

  goto exit;
  exit:
}
