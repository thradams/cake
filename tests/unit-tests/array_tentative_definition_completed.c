/*
  An array of unknown size is compatible with one of known size, and the
  composite type is the one with the known size (6.2.7). At file scope a
  tentative definition can be completed by a later declaration, so uses of
  the name must see that size - the same way an incomplete tag is completed
  by a later definition. Issue #333.

  The first declaration is the one registered in the symbol table; it points
  to the declaration that completes it, and the type reaches it through its
  declarator (see type_get_complete_array, which mirrors
  get_complete_struct_or_union_specifier).
*/

/* completed by the initializer: the size is only known after it is parsed */
char s[];
char s[] = { "123" };
static_assert(sizeof(s) == 4);

/* completed by an explicit size */
int a[];
int a[2];
static_assert(sizeof(a) == sizeof(int) * 2);

/* the completing declaration may also come first */
int b[3];
int b[];
static_assert(sizeof(b) == sizeof(int) * 3);

/*
  A type taken from an incomplete array keeps pointing at the declarator it
  came from, not at the one being declared - it is 'c' that gets completed,
  not 'd' or the return type of 'pc'.
*/
int c[];
typeof(c) d;
typeof(c) * pc();
int c[2];
static_assert(sizeof(d) == sizeof(int) * 2);
static_assert(sizeof(*pc()) == sizeof(int) * 2);

/*
  A parameter of array type is adjusted to a pointer (6.7.6.3), so its size is
  not part of the function type: all of these declare the same function, and
  none of them is an error.

  Cake still reports the mismatch, because the sizes were probably meant to
  agree. It is only a warning and can be turned off with -wd52.
*/
void f(int a[]);
void f(int a[2]); //lint 52 declared with array parameter of different size

void g(int a[2]);
void g(int a[3]); //lint 52 declared with array parameter of different size

/* two unspecified sizes agree, so nothing is reported */
void k(int a[]);
void k(int a[]);


int main()
{
    /* a function can be redeclared in any scope, so the warning is too */
    void h(int a[3]);
    void h(int a[1]); //lint 52 declared with array parameter of different size
}
