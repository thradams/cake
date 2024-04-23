#pragma ownership enable
#pragma nullable enable

struct X {
  char *_Owner name;
};

struct X * _Owner f();
int main(){
    struct X * _Owner p = f();
    void * _Owner p2 = p;
}