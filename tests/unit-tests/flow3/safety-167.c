#pragma safety enable

void f(char* p)
{
  /* FIXED: `while (*p) { p++; }` no longer degrades p to ANY on the second time around, no false null dereference */
  while (*p) { p++; } // ok
}
