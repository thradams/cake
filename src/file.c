enum E1 { A };
enum E2 { B };

int main() {
#pragma CAKE diagnostic warning "-Wno-enum-compare"
    if (A == B){}
#pragma CAKE diagnostic warning "-Wenum-compare"
    if (A == B) {}

    #warning aasd sadasdas

}
