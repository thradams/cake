

#pragma cake diagnostic check "-E74"

struct X{    
    int b[sizeof(struct X)];
};
int main(){}