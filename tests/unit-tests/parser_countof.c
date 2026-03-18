#include <stdio.h>

/*
 * Tests for _Countof (C2Y).
 *
 * Compile-time cases use static_assert — a wrong result is a compile error.
 * Runtime cases (VM arrays) use a plain check() with a failure counter.
 *
 * Cases covered:
 *   1. _Countof(expression)  — 1-D array
 *   2. _Countof(expression)  — multi-dimensional array (outermost dim only)
 *   3. _Countof(type)        — array type
 *   4. _Countof(expression)  — enum variable
 *   5. _Countof(type)        — enum type
 *   6. _Countof(expression)  — VM array (runtime dimension, pointer-to-VM)
 *   7. _Countof(expression)  — VM array passed as function parameter
 */

/* -------------------------------------------------------------------------
 * Enums used across multiple tests
 * ------------------------------------------------------------------------- */

enum Color     { RED, GREEN, BLUE };
enum Direction { NORTH, SOUTH, EAST, WEST };

/* -------------------------------------------------------------------------
 * 1. _Countof on a 1-D array expression
 * ------------------------------------------------------------------------- */

static int a[5];
static_assert(_Countof(a) == 5, "_Countof 1D array expression");

/* -------------------------------------------------------------------------
 * 2. _Countof on a multi-dimensional array — outermost dim only
 * ------------------------------------------------------------------------- */

static int m[3][7];
static_assert(_Countof(m)    == 3, "_Countof 2D array outermost dim");
static_assert(_Countof(m[0]) == 7, "_Countof 2D array inner dim");

/* -------------------------------------------------------------------------
 * 3. _Countof on an array type
 * ------------------------------------------------------------------------- */

static_assert(_Countof(int[4])    == 4, "_Countof 1D array type");
static_assert(_Countof(int[6][2]) == 6, "_Countof 2D array type outermost");

/* -------------------------------------------------------------------------
 * 4. _Countof on an enum expression
 * ------------------------------------------------------------------------- */

static enum Color     s_color = RED;
static enum Direction s_dir   = NORTH;
static_assert(_Countof(s_color) == 3, "_Countof enum expression 3 members");
static_assert(_Countof(s_dir)   == 4, "_Countof enum expression 4 members");

/* -------------------------------------------------------------------------
 * 5. _Countof on an enum type
 * ------------------------------------------------------------------------- */

static_assert(_Countof(enum Color)     == 3, "_Countof enum type 3 members");
static_assert(_Countof(enum Direction) == 4, "_Countof enum type 4 members");

/* -------------------------------------------------------------------------
 * Runtime helpers (VM cases only)
 * ------------------------------------------------------------------------- */

static int failures = 0;

static void check(const char* name, int ok)
{
    if (ok) printf("PASS  %s\n", name);
    else  { printf("FAIL  %s\n", name); failures++; }
}

/* -------------------------------------------------------------------------
 * 6. _Countof on a VM array via pointer-to-VM (runtime dimension)
 *
 *    int (*p)[n] — _Countof(*p) must return n at runtime.
 * ------------------------------------------------------------------------- */

static void test_vm_pointer(int n)
{
    int (*p)[n] = (void*)0; /* only the type matters for _Countof */
    check("VM pointer-to-array countof", _Countof(*p) == (size_t)n);
}

/* -------------------------------------------------------------------------
 * 7. _Countof on a VM array parameter
 *
 *    The outermost dimension decays to a pointer when passed as a parameter
 *    so only the inner fixed dimension is valid for _Countof here.
 * ------------------------------------------------------------------------- */

static void check_param_countof(int arr[][4])
{
    check("VM param inner dim", _Countof(arr[0]) == 4);
}

static void test_vm_parameter(void)
{
    int data[6][4];
    check_param_countof(data);
}

/* -------------------------------------------------------------------------
 * main
 * ------------------------------------------------------------------------- */

int main(void)
{
    /* compile-time cases already verified above by static_assert */

    test_vm_pointer(8);
    test_vm_parameter();

    if (failures == 0)
        printf("\nAll tests passed.\n");
    else
        printf("\n%d test(s) FAILED.\n", failures);

    return failures != 0;
}