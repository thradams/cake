typedef typeof(U' ') char32_t;
int main() {
    [[cake::w41]]
    char32_t c = U'\U00110000'; // Out-of-range for UTF-32
    return 0;
}
