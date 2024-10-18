#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>
#include <stdio.h>

ssize_t my_write(int fd, const void *buf, size_t count)
{
	return syscall(SYS_write, fd, buf, count);
}

int main()
{
	const char *message = "Hello, world\n";
	size_t length = strlen(message);
	
	ssize_t bytes;

	bytes = my_write(1, message, length);
	if(bytes == -1)
	{
		printf("write calling error\n");
	}
	return 0;
}
