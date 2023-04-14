
int main() {
    const int* const p;
    static_assert(_Generic(p, const int*  : 1, default: 0));
}
