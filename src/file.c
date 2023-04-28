void F(int a[static 5]) {
}
int main() {
    
    F(0);

    int a[] = {1, 2, 3};    
    F(a);
    
    int b[] = { 1, 2, 3 , 4, 5};    
    F(b);

}