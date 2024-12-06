
struct X {
  int i;
};

int main(){
   struct X x = {};
   if ((struct X){}.i)
   {}
}
