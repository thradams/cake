#pragma safety enable

struct X {
  int * _Opt data;
  bool has_pointer;
};

void f(struct X* p)
{
    if (p->has_pointer)
    {
        _Assert(p->data); /* when has_pointer is true, p->data is not null*/
        *p->data = 1;
    }
}

int main()
{
   _Assert(1 == 1);
}

