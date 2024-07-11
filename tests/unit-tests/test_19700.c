int main(){

    int x = 1;
    auto * p = &x;
#pragma cake diagnostic check "-E129"
}

