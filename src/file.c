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

void clear(struct X* p3)
  post(is_empty(p3))
{
    p3->data = 0;
}

void push(struct X* p1)
  post(!is_empty(p1))
{
}

void f(struct X* pX)
{     
    push(pX); 
    static_state(pX->data, "not-null");
    clear(pX);
    static_state(pX->data, "null");
}
