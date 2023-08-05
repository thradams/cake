void * _Owner malloc(int i);

struct Y {
  struct X * _Owner x;
};

void x_delete(_Implicit struct X  * _Owner p);

void f(struct Y * p, int condition)
{
     struct X * _Owner x = malloc(1);
     p->x = _Move x;
     x = 0;

     if (condition)
     {
     }

     x_delete(x);
}
 