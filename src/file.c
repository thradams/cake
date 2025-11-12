int main(){
  static_assert("\300\040\040"[0] ==-64);  
  static_assert('\300' == 192);
}
