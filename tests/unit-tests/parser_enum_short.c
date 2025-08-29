enum X : short {
    A
};

int main() {
    enum X x = A;
    static_assert(sizeof(x) == sizeof(short));
    static_assert(sizeof(enum X) == sizeof(short));

    static_assert(alignof(x) == alignof(short));
    static_assert(alignof(enum X) == alignof(short));
}
