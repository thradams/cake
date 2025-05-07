int main(){
    static_assert(0 ? 1/0 : 1);
    static_assert(1 ? 1 : 1/0);
}