#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void function()
{
	char *buffer = malloc(100);
	if(buffer == NULL) 
	{
		printf("Memory allocation error\n");
		return;
	}
	snprintf(buffer, 100, "hello world");
	printf("%s\n", buffer);
	free(buffer);
	printf("%s\n", buffer);

	char *buffer2 = malloc(100);
	if(buffer2 == NULL)
	{
		printf("Memory allocation error\n");
		return;
	}
	snprintf(buffer2, 100, "hello world 2");
	printf("before moving: %s\n", buffer2);
	buffer2 = buffer2 + 2;
	printf("after moving: %s\n", buffer2);
	free(buffer2); // free(buffer2 - 2) - аварийной остановки не будет
	printf("after free: %s\n", buffer2);
}
/*
 из-за малеького размера строки при смещении указателя на середину буфера printf ничего не выведет, т.к. зайдет за нуль-терминатор, если сместить указатель на небольшое значение в пределах длины строки, то 
 printf выведет часть строки до конца

 аварийная остановка происходит потому что передаваемый указатель не является действительным указателем на выделенный кусок памяти
 */
int main()
{
	function();
	return 0;
}
