static_assert(1+ 2);
static_assert(1 + (2+3), "message");

int main() {
    static_assert(1);
    static_assert(1, "message");
    static_assert(1), 0;
}
