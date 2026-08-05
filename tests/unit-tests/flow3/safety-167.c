#pragma safety enable

void f(char* p)
{
  /* FIXED: this used to warn "possible null pointer dereference" on
     *p above, once execution went around the loop a second time --
     p++ degraded p's tracked "definitely non-null" state to a generic
     ANY value (treated as "could be zero"), even though incrementing a
     valid pointer can never make it null. */
  while (*p) { p++; } // ok
}
