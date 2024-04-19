#pragma nullable enable

char* _Opt f() {
    char* _Owner _Opt p = 0;
    return p;
#pragma cake diagnostic check "-Wnon-owner-move"
}
