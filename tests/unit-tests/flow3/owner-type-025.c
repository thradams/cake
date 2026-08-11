struct X { const char* _Owner text; };
void destroyX(struct X x) 
{
	_Assert(x.text == 0);
}

int main() 
{
}
