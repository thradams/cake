#pragma flow enable

int *f();
int main()
{      
   if (int *p = f())
   {
     static_state(p, "not-null");
   }
}
