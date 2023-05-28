
enum E1 { A };
enum E2 { B };

int main() {
#if 0
#pragma CAKE diagnostic push
#pragma CAKE diagnostic warning "-Wno-enum-compare"
    if (A == B){}
#pragma CAKE diagnostic pop

    if (A == B) {}
#endif
}
