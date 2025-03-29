void f(double ){}
int main(void)
{
  _Generic((1.0),  float: f , default: f, default: f )(1.0);
}
#pragma cake diagnostic check "-E1570"