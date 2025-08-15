
struct X {
        int x;
};

int main() {
    constexpr struct X x = (struct X){ .x = 50 };
    static_assert(x.x == 50);
}