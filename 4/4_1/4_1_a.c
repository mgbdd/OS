#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int globalInit = 1; 
int globalUninit; 
const int globalConst = 2; 

void printAddress()
{    
    int localVar = 3;
    static int staticVar = 4; 
    const int constVar = 5; 

    printf("local var = %p\n", (void*)&localVar);
    printf("static var= %p\n", (void*)&staticVar);
    printf("const var = %p\n", (void*)&constVar);
}

int main()
{    
    printf("global init = %p\n", (void*)&globalInit);
    printf("global uninit = %p\n", (void*)&globalUninit);
    printf("global const = %p\n", (void*)&globalConst);

    printAddress();

	char mapsPath[100];
	snprintf(mapsPath, sizeof(mapsPath), "/proc/%d/maps", getpid());
       FILE *mapsFile = fopen(mapsPath, "r");

	if(mapsFile == NULL)
	{
		printf("File opening error\n");
		return 0;
	}

	char *line = malloc(256);
	while(fgets(line, sizeof(line), mapsFile) != NULL)
	{
		printf("%s", line);
	}
	fclose(mapsFile);
    return 0;
}
