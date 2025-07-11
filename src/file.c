
void f3(){
    static int i = 1;
    (void(void)){ i = 2; }();
}