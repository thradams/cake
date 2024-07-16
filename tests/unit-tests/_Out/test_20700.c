#pragma safety enable

struct X {
  int * /*_Opt*/ p;
};

struct X * make();

void f(struct X *p,  int * p);

int main()
{
    struct X *  pX = make();
    if (pX->p)
    {
       //cake is making pX->p  unkown before function call..it must be after
       f(pX, pX->p);
    }
}
