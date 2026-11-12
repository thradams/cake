#pragma flow enable

/* Without check_annotations there is no _Out and no _Opt, so the flow analysis
   does not assume the worst about what it cannot see:

   - a non-const pointer parameter may only write the pointee (it could be an
     out parameter), so passing an uninitialized object to it is not reported;
     a const one can only read it, so that is reported.

   - a pointer parameter is not known to be null (without default_nonnull it
     may or may not be), so dereferencing it is not reported; a null the flow
     has seen (p = 0) is.
*/

void writes(int a[2]);
void reads(const int a[2]);
void writes_int(int* p);

void counts(int* n)
{
    *n = 0; /* n is unknown, not possibly null */
}

int main(void)
{
    int x[2];
    writes(x); /* may only write it */

    int y[2];
    reads(y); //lint 30 passing a possibly uninitialized object 'y' (see line 29)

    int z;
    writes_int(&z); /* z may be initialized by the call */

    int* p = 0;
    *p = 1; //lint 33 possible null pointer dereference '*p'

    return z;
}
