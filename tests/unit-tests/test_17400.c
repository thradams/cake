#pragma nullable enable
#pragma ownership enable

struct X {
    int i;
};

void f(struct X * _Opt p)
{
      if (p && p->i)
      {          
      }
      else
      {          
      }
}
