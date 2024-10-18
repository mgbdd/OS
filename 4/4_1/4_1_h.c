#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//export MY_VARIABLE=envvar

int main()
{
	printf("%d\n", getpid());
	char *myVar = getenv("MY_VARIABLE");
	if(myVar != NULL)
	{
		printf("var value: %s\n", myVar);
		printf("pointer address: %p\n", &myVar);
		printf("address of env var: %p\n", myVar);
	}	
	else{
		printf("Enviromental variable can not be found\n");
	}
	
	if(setenv("MY_VARIABLE", "new value", 1) != 0)
	{
		printf("setenv error\n");
	}

	myVar = getenv("MY_VARIABLE");
	if(myVar != NULL)
	{
		printf("%s\n", myVar);
		printf("%p\n", myVar);
	}
	else{
		printf("Enviromental variable can not be found\n");
	}

	//sleep(600);
	return 0;
}
