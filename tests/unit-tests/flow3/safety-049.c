#pragma safety enable

struct X {
  int * p;
};

struct X * _Nonnull make();

void f(struct X *p,  int * p2);

int main()
{
    struct X *  pX = make();
    if (pX->p)
    {
       //cake is making pX->p  unkown before function call..it must be after
       f(pX, pX->p);
    }
}
