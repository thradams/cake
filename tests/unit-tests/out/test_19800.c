#pragma safety enable

struct X {
  char */*_Owner*/ /*_Opt*/ name;
};

struct X * /*_Owner*/ make();
void del(struct X * /*_Owner*/ /*_Opt*/ p);

int main() {
   struct X * /*_Owner*/ p = make();
   p++;
#pragma cake diagnostic check "-E130"

   p--;
#pragma cake diagnostic check "-E131"

   del(p);
}


