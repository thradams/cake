#pragma safety enable
int f(int condition){
  int * _Owner _Opt p = 0;
  try
  {
    int * _Owner _Opt p2 = p;
    if (condition) throw;
    p = 0;
  }
  catch{}
  static_state(p, "null moved"); //shows "null" - it should be "null or moved"
}
//error in flow analysis#117