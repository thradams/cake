
#pragma safety enable

void consume(int *_Owner);
void use(int *);

void test(int *_Owner x) 
{
    int *y = x;
    consume(x);
    *y = 0; //lint 31
    use(y); //missing warning
}