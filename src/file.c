#pragma nullable enable

void f(int * p, int *p2) 
{
   if (p || p2)  //warning p is not-null
   {
   }
   if (p && p2)  //warning p is not-null
   {
   }
}

int main()
{ 
  int * p = nullptr; //warning p is non-nullable
}
