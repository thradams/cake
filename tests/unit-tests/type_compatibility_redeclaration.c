/*
  https://github.com/thradams/cake/issues/226
  Function redeclarations: top-level parameter qualifiers don't affect the
  function type, but qualifiers of what a parameter points to do.
*/

/* compatible: top-level parameter qualifier is not part of the function type */
void f(int);
void f(volatile int);

void g(int *);
void g(int * const p);

/* incompatible: pointee qualifier differs -> conflicting redeclaration */
void h(int *);
void h(const int *p); //lint 1020 redeclaration
