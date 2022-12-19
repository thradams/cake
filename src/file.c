
int main() {
    
    void (*f[])(int i[]) = {1, 2, 3};
    
    static_assert(sizeof(f) == sizeof(void*) * 3);
}
