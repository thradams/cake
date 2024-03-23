struct X {int i;};

void f(struct X * p){}

struct X * make(){
      struct X * p = 0;
      return p;
}

int main() {
  //struct X * p = 0;//make();
  //f(p);
}
