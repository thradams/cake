/*
  C2Y (N3580) limits the second clause of a selection-header to an expression.
  The C++ form with a second declaration is not accepted.
*/

int main()
{
    if (int i = 0; int k = 2) //lint 2050
    {
    }
}
