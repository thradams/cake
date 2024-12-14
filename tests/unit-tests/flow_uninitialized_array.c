#pragma safety enable

void f_const(const char *s);

void f_non_const(char *s);

int main(void)
{
  char buffer[20];
  
  f_const(buffer);  
#pragma cake diagnostic check "-Wanalyzer-maybe-uninitialized"
#pragma cake diagnostic check "-Wanalyzer-maybe-uninitialized"

  static_debug_ex(buffer);

  f_non_const(buffer);  
  #pragma cake diagnostic check "-Wanalyzer-maybe-uninitialized"
  #pragma cake diagnostic check "-Wanalyzer-maybe-uninitialized"
}