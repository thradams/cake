struct X { const char* _Nonnull _Owner text; };
void destroyX(struct X x) 
{
	_Assert(x.text == 0);
} //lint 29 _Assert assumes nothing

int main() 
{
}
