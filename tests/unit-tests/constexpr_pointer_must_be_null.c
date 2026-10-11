// constexpr pointer: the only value allowed is a null pointer constant

int g;

constexpr int* p1 = &g;      //lint 2380
constexpr int* p2 = 0;       // OK
constexpr int* p3 = nullptr; // OK
const int* p4 = &g;          // OK: const pointer may hold an address

int main() {
    return 0;
}
