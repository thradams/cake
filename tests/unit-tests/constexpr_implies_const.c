/*
   constexpr must imply const on the declared object (C23 6.7.1), so that
   writing through the declared identifier is diagnosed as a modifiable-lvalue
   violation. For an array, the const applies to the (possibly nested)
   element type rather than to the array type itself.
*/

void array_element(void)
{
    constexpr int a[] = { 1, 2 };
    a[1] = 2; //lint 920 error: assignment of read-only object
}

void scalar(void)
{
    constexpr int x = 5;
    x = 6; //lint 920 error: assignment of read-only object
}

void struct_member(void)
{
    struct P { int x; int y; };
    constexpr struct P p = { 1, 2 };
    p.x = 3; //lint 920 error: assignment of read-only object
}

void typeof_reports_const(void)
{
    constexpr int a = 1;
    static_assert(_Generic(typeof(a), const int : 1));
}
