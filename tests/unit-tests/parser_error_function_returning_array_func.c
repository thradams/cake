//error: function returning function

void (*p)(int, int)(int, int); //lint 1410

//error: function returning array

void (*p2)(int, int)[10]; //lint 1420


