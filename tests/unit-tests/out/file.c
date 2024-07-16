#pragma ownership enable
#pragma nullable enable

struct X {
  char */*_Owner*/ name;
};

void x_destroy(_Opt struct X * /*_Obj_owner*/ x) 
{
  //free(x.name);
}