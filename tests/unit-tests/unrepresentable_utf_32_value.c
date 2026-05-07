typedef typeof(U' ') char32_t;
int main() {
    
    char32_t c = U'\U00110000'; //lint 41 Out-of-range for UTF-32
    return 0;
}
