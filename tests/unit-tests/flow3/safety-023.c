#pragma safety enable


struct X {
    int i;
};

void f(const struct X * _Opt p)
{
      if (p && p->i)
      {          
      }
      else
      {          
      }
}
