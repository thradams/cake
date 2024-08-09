char abc[3] = "abcd";
#pragma cake diagnostic check "-Warray-size"
char abcd[4] = "abcd";
#pragma cake diagnostic check "-Warray-size"

