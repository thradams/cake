void (*p)(int, int)(int, int);
#pragma cake diagnostic check "-E1410"

void (*p2)(int, int)[10];
#pragma cake diagnostic check "-E1420"
