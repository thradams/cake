
#pragma CAKE nullchecks ON

void f(int * p)
{
  static_state(p, "not-null");
}

//#pragma CAKE nullchecks OFF

void f(int * p)
{
  static_state(p, "maybe-null");
}

int main()
{

}
