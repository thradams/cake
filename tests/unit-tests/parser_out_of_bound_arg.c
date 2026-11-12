// In C an array parameter is a pointer: `const int array[5]` is
// `const int* array`, and the caller may pass an array of any size.
//
// We assume more: the size written in the parameter is the size of the
// object it points to. So the parameter is treated as an object with that
// number of elements, and a constant index is checked against it.
//
// Without a size (`array[]`, or a pointer) nothing is assumed: the object is
// not known and the index is not checked.

void sized(const int array[5])
{
    int first = array[0];   /* ok */
    int last = array[4];    /* ok: the last element */
    int past = array[5];    //lint 42
    int far = array[100];   //lint 42
    int before = array[-1]; //lint 42
}

void sized_write(int array[5])
{
    array[4] = 0; /* ok */
    array[5] = 0; //lint 42
}

/* `static 5` promises at least 5 elements; we assume exactly 5, as above */
void sized_static(const int array[static 5])
{
    int last = array[4]; /* ok */
    int past = array[5]; //lint 42
}

void unsized(const int array[])
{
    int i = array[5];   /* ok: the size is not known */
    int j = array[100]; /* ok */
}

void pointer(const int* p)
{
    int i = p[5]; /* ok: the size is not known */
}

void two_dimensions(const int m[2][3])
{
    int ok = m[1][2];     /* ok: the last element */
    int row = m[2][0];    //lint 42
    int column = m[1][3]; //lint 42
}

void parenthesized(const int array[5])
{
    int past = (array)[5]; //lint 42
}

void not_evaluated(const int array[5])
{
    int size = sizeof(array[5]); //lint 42
}
