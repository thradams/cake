void F(int a[static const 5]) {
    static_assert( (typeof(a)) == (int* const));
    a = 1;
}
int main() {
    
    

}