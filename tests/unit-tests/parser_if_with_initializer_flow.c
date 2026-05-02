#pragma flow enable

int *f();
int main()
{      
   if (int *p = f())
   {
     assert_state(p, "not-null");
   }
}
