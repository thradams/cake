#pragma  safety enable

struct X {
  char* _Owner  text;
};

void destroy_x(_Dtor struct X *p)
{
   /*
      _Dtor we consider that all members of p CAN  be null
      the rationally is to allow partically created object be destroyed

      ...but "can be null" is not "can be forgotten": the _Dtor contract
      excuses the CALLER from releasing x.text (the call at line 18 ends
      its lifetime), so the CALLEE must actually release it. Nothing
      here does, so this is a leak.
   */
} //lint 29 owner object (.text) not moved (root see line 7)

int main()
{
   struct X x = {0};
   destroy_x(&x);
}

