#pragma safety enable

struct X {
    int* _Opt data;
};

bool is_empty(struct X* p2)
  true(p2->data == nullptr),
  false(p2->data != nullptr)
{
    return p2->data == nullptr;
}

void push(struct X* p1)
  post(p1->data != nullptr)
{
}

void f(struct X* pX)
{     
    push(pX); 
    static_state(pX, "not-null");
}
