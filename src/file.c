enum X : short {
    A
};

int main() {
    enum X x = A;
    static_assert(sizeof(x) == sizeof(short));
}
