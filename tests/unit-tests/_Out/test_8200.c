#pragma safety enable



void* f();
int main() {
    void* /*_Owner*/ p = f();
   #pragma cake diagnostic check "-Wnon-owner-to-owner-move"
}

#pragma cake diagnostic check "-Wmissing-destructor"
