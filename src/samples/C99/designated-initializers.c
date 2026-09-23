

struct T {
    int k;
    int l;
};

struct S {
    int i;
    struct T t;
};

struct T x = {
    .l = 43,
    .k = 42,
};

void f(void) {
    struct S l = {
        1,
        .t = x,
        .t.l = 41,
    };
}

int main() {
    int a[6] = {[4] = 29, [2] = 15};

    struct point {
        int x, y;
    };
    struct point p = {.y = 2, .x = 3};

    struct {
        int a[3], b;
    } w[] = {[0].a = {1}, [1].a[0] = 2};
}

#define A_MAX 20
int a[A_MAX] = { 1, 3, 5, 7, 9, [A_MAX-5] = 8, 6, 4, 2, 0 };
