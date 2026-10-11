// constexpr object cannot have volatile-qualified members

struct S { volatile int v; };
struct T { struct S s; };

constexpr struct S s = { 1 };    //lint 2340
constexpr struct T t = { { 1 } }; //lint 2340
constexpr struct S a[1] = { { 1 } }; //lint 2340

int main() {
    return 0;
}
