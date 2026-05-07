struct S {
    int a;
};
int main() {
    union S {
        int i;
    };

    /*we cannot catch this error because it throw before commnet parsing*/
 
   // struct S x2;
}