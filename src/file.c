int main(){
  static_assert("\100\040\040"[0] == 64);
  static_assert('\100' == 64);

}