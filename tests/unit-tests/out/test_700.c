enum E { A };
struct E { int i; };
// error: use of 'E' with tag type that does not match previous declaration.
#pragma cake diagnostic check "-E102"
