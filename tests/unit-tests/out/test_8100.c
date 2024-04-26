#pragma safety enable



int* /*_Owner*/ make1();
int* /*_Owner*/ /*_Opt*/ make2();
void free(void * /*_Owner*/ /*_Opt*/ p);


void f(int condition)
{
  int * /*_Owner*/ /*_Opt*/ p = 0;
  

  if (condition)
  {
       
       p = make1();
       
       free(p);
       p = make2();
       
  }
  else
  {
    
  }
  free(p);
}