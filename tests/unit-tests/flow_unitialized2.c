#pragma flow enable

void f_const(const char *s);

void f_non_const(char *s);

int main(void)
{
  char buffer[20];
  
  f_const(buffer);  
#pragma cake diagnostic check "-Wanalyzer-maybe-uninitialized"
#pragma cake diagnostic check "-Wanalyzer-maybe-uninitialized"

  f_non_const(buffer);  
  //now warning in flow mode, warning in safety mode
  #pragma cake diagnostic check "-Wanalyzer-maybe-uninitialized"
}