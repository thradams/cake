#pragma safety enable

int g = 1;

void f1()
{
     //externa non constant variable are any state
     assert_state(g, "any");
     g  = 1;
     assert_state(g, "not-zero");
     g = 0;
     assert_state(g, "zero");     
}

void f2()
{
     assert_state(g, "any");
     g = 2;
     g = 3;
}
