#pragma  safety enable

struct X {
  char* _Owner  text;
};

void destroy_x(_Dtor struct X *p)
{
   /* _Dtor: members of p can be null (partially built objects), but the callee must still release x.text, nothing here does */
} //lint 29 owner object (.text) not moved (root see line 7)

int main()
{
   struct X x = {0};
   destroy_x(&x);
}

