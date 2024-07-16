#pragma safety enable

int g = 1;

void f1()
{
     //externa non constant variable are any state
     
     g  = 1;
     
     g = 0;
          
}

void f2()
{
     
     g = 2;
     g = 3;
}
