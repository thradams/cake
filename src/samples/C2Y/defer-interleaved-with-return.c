
int f(){
  int i = 1;
  _Defer {
    i = 0;
  }
  return i++;
}

void f0(){
  int i = 1;
  _Defer {
    i = 0;
  }
  return; //empty or constant expression
}
