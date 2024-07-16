#pragma safety enable

void F(char * s){}
int main()
{ 
  char buffer[2] = {0};
  F(buffer+1);
}
