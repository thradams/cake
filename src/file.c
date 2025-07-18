enum foo: unsigned long long;

void foo(enum foo);

void bar(enum foo x) {
    foo(x);
}