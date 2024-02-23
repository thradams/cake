enum E1 { A };
enum E2 { B };

int main() {

#pragma CAKE diagnostic push
#pragma CAKE diagnostic error "-Wenum-compare"
    if (A == B) {}
#pragma CAKE diagnostic pop

#pragma CAKE diagnostic push
#pragma CAKE diagnostic warning "-Wenum-compare"
    if (A == B) {}
#pragma CAKE diagnostic pop

#pragma CAKE diagnostic push
#pragma CAKE diagnostic note "-Wenum-compare"
    if (A == B) {}
#pragma CAKE diagnostic pop


#pragma CAKE diagnostic push
#pragma CAKE diagnostic ignored "-Wenum-compare"
    if (A == B) {}
#pragma CAKE diagnostic pop




}
