//error: function returning function
//error: storage size of 'p' isn't known
[[cake::e1270]] [[cake::e1410]]
void (*p)(int, int)(int, int);

//error: function returning array
//error: storage size of 'p2' isn't known
[[cake::e1270]]
[[cake::e1420]]
void (*p2)(int, int)[10];


