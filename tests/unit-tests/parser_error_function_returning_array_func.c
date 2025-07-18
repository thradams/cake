void (*p)(int, int)(int, int);
#pragma cake diagnostic check "-E1410"
#pragma cake diagnostic check "-E1270"

void (*p2)(int, int)[10];
#pragma cake diagnostic check "-E1420"
#pragma cake diagnostic check "-E1270"

