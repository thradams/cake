#pragma safety enable


void free(void* /*_Owner*/) { }
#pragma cake diagnostic check "-Wmissing-destructor"
