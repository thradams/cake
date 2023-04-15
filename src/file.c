struct [[destroy]] X { int i;  };
int main() {    
    struct X x;

    _del_attr(x, "must destroy");
}