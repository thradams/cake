/*
  A braced initializer gets the same assignment checks as `T a = b;`.
  object_set does no type checking, so braced_initializer_new used to accept
  these silently -- found via `-const-literal` with `struct X x = {""};`.
  Spelled with an explicit `const char*` here so the test needs no flag.
*/

struct X { char* s; };

const char* p;

int main()
{
    struct X x = { p };        //lint 15
    struct X x2 = { .s = p };  //lint 15
    char* s = { p };           //lint 15

    /*
      C23 6.7.11p14: an array of character type initialized by a string
      literal is a special initializer form, not an assignment, so no
      qualifier check applies here -- not even with -const-literal.
    */
    char a[2] = "";
    char b[] = "ab";
    struct Y { char a[2]; };
    struct Y y = { "" };
    static_assert(sizeof(b) == 3);
}
