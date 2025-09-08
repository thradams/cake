// File scope declaration with attribute
[[deprecated("use g2 instead")]]
int g1 = 42;

int g2 = 10;

[[nodiscard]] int f([[maybe_unused]] int x)
{
    // Attribute on a local variable
    [[maybe_unused]] int local = 123;

    // Attribute inside a statement
    [[likely]] if (x > 0) {
        return g1;
    }

    [[unlikely]] else {
        return g2;
    }
}

// Attribute before a struct definition
[[gnu::packed]]
struct S {
    int a;
    [[deprecated]] int b;  // Attribute on a member
};

// Attribute on a typedef
typedef [[nodiscard]] int myint;

// Attribute on an enumerator
enum E {
    A [[deprecated]],
    B [[maybe_unused]]
};

// Attribute in a parameter declaration
void h(int n, [[maybe_unused]] double y);

// Attribute in a label statement
void test_label(void) {
    goto L;
    [[maybe_unused]] L: ; // attribute on label
}

// Attribute before an expression statement
void test_expr(void) {
    [[maybe_unused]] (void)f(1);  // expression statement
}

// Attribute on alignment specifier
alignas(16) [[maybe_unused]] char buffer[64];

int main(void) {
    f(1);
    return 0;
}
