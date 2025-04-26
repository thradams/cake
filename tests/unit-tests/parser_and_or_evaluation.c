int main(){
  int i;
  static_assert((1 || i) == 1);
  static_assert((0 && i) == 0);
}
