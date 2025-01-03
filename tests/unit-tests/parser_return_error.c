void f(){
    return 1;
}

#pragma cake diagnostic check "-E1120"


int f2() {
    return;
}

#pragma cake diagnostic check "-E1121"

