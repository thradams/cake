enum E1 { A };
enum E2 { B };

int main() {

#pragma CAKE diagnostic push
#pragma CAKE diagnostic warning "-Wno-enum-compare"
    if (A == B){}
#pragma CAKE diagnostic pop

    if (A == B) {}

    #warning aasd sadasdas

}
