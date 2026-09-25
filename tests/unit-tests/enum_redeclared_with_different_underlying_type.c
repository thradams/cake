enum E : int;
enum E { A }; //lint 950 enum 'E' redeclared with a different underlying type

enum F : int;
enum F : short { B }; //lint 950 enum 'F' redeclared with a different underlying type

int x = A + B;
