enum X : short {
    A
};

int main() {
    static_assert(sizeof(A) == 2);
}