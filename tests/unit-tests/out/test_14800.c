struct X { const char* /*_Owner*/ text; };
void destroyX(struct X x) 
{
	assert(x.text == 0);
}

int main() 
{
}
