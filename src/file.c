void* f();
int main() {
    void* _Owner p = f();
#pragma cake diagnostic check "-Wmissing-owner-qualifier"
}

#pragma cake diagnostic check "-Wmissing-destructor"