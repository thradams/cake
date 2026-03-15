
int f(int x);

int test_simple(int a, int b) {
    return a ?: b;
}

int * test_pointer(int * p, int * fallback) {
    return p ?: fallback;
}

int test_side_effect(int i, int b) {

    return i++ ?: b;
}

int test_call(int b) {
    return f(1) ?: b;
}
int test_nested(int a, int b, int c) {
    return a ?: (b ? b : c);
}

int test_return(int x) {
    return x ?: 42;
}

static_assert( 1?: 0 == 1);
