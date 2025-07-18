struct X {
    int i;
};


void f(struct X * p);
void f2(struct X * p);

void delete(struct X * p)
{
    f(p);
    f2(p);
}

