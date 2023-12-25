struct X
{
    char* _Owner s;
};
void init(struct X * _Out x){}

int main() {
    struct X x;
    init(&x);
}