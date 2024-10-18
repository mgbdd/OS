#include <stdio.h>

void hello_from_static_lib();

int main()
{
	printf("Hello world\n");
	hello_from_static_lib();
	return 0;
}

