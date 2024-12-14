#pragma safety enable

int g = 1;

void f1()
{
     //externa non constant variable are any state
     static_state(g, "any");
     g  = 1;
     static_state(g, "not-zero");
     g = 0;
     static_state(g, "zero");     
}

void f2()
{
     static_state(g, "any");
     g = 2;
     g = 3;
}
