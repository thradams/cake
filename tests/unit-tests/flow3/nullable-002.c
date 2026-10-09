#pragma default_nonnull
#pragma check_annotations enable
#pragma flow enable

void receives_non_null(char* p);

int main()
{
    receives_non_null(0); //lint 35 cannot convert a null pointer constant to non-nullable pointer; passing a possible null
}

