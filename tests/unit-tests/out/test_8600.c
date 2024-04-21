#pragma nullable enable
#pragma ownership enable

void free(void* /*_Owner*/) { }
#pragma cake diagnostic check "-Wmissing-destructor"
