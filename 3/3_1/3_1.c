#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

char *reverseFileName(char *line)
{       
	int lineLen = strlen(line);
	char *new = calloc(lineLen + 1, sizeof(char));
	
	char *dot = strchr(line, '.');
	if(dot == NULL) //случай без расшиения
	{
		for(int i = 0; i < lineLen; i++)
		{
			new[i] = line[lineLen - i - 1];
		}
		return new;
	}
	else
	{

	int dotPos;
	for(int i = lineLen - 1; i >= 0; i--) //нашли точку, разделяющее расширение и имя файла
	{
		if(line[i] == '.') 
		{
			dotPos = i;
			break;
		}
	}
	printf("%i\n", dotPos);
	for(int i = 0; i < dotPos; i++)
	{
		new[i] = line[dotPos - i - 1];
	}
	for(int i = dotPos; i < lineLen; i++)
	{
		new[i] = line[i];
	}

	}
	return new;
	
}

void copyFile(char *srcPath, char *dstPath, struct stat srcStat)
{

	

	FILE *srcFile = fopen(srcPath, "r");
	if(!srcFile)
	{
		printf("Failed to open source file\n");
		return;
	}
	fseek(srcFile, 0, SEEK_END);
	long fileSize = ftell(srcFile);
	fseek(srcFile, 0, SEEK_SET);


	FILE *dstFile = fopen(dstPath, "w");
	if(!dstFile)
	{
		printf("Failed to create destination file in copy file\n");
		fclose(srcFile);
		return;
	}
	
	size_t readSize = fileSize / 5; 
	char *buffer = malloc(readSize);
	size_t bytesToRead = fileSize;

	
	while (bytesToRead > 0) 
	{
        	size_t bytesRead = fread(buffer, 1, readSize, srcFile);
        	for (size_t i = 0; i < bytesRead / 2; i++) 
		{
            		char temp = buffer[i];
            		buffer[i] = buffer[bytesRead - i - 1];
            		buffer[bytesRead - i - 1] = temp;
        	}
        	fwrite(buffer, 1, bytesRead, dstFile);
        	bytesToRead -= bytesRead;
    }

	/*while(fileSize > 0)
	{
		bytesRead = fread(buffer, 1, readSize, srcFile);
		for(int i = fileSize - 1; i >= fileSize - readSize; i--)
		{
			fwrite(&buffer[abs(i - fileSize + 1)], 1, 1, dstFile);
		}
		fileSize -= readSize;
		printf("bytesRead %ld, fileSize %li\n", bytesRead, fileSize);
	}*/



	if(chmod(dstPath, srcStat.st_mode) == -1)
	{
		perror("Failed to copy file permission\n");
		return;
	} 
	
	free(buffer);
	fclose(srcFile);
	fclose(dstFile);
}

void openDir(char *src)
{
	char dst[strlen(src)];
	dst[0] = '.';
	dst[1] = '/';
	//src len = 3
	for(int i = strlen(src) - 1; i>= 0; i--)
	{
		dst[strlen(src) - i + 1] = src[i];
	}
	
	DIR *srcDir = opendir(src);
	if(!srcDir)
	{
		printf("Failed to open source directory\n");
		return;
	}

	if(mkdir(dst, 0755) == -1)
	{
		printf("Failed to create destination directory\n");
		closedir(srcDir);
		return;
	}

	struct dirent* entry;
	while((entry = readdir(srcDir)) != NULL)
	{
		if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
		{
			continue;
		}

		char srcPath[1024];
		snprintf(srcPath, sizeof(srcPath), "%s/%s", src, entry->d_name);
		struct stat fileStat; //stat(srcPath, &fileStat);
		if(stat(srcPath, &fileStat) == -1)
		{
			perror("Failed to get file permission\n");
			return;
		}

	
		if(S_ISREG(fileStat.st_mode))//если файл регулярный, надо скопировать его
		{	
			printf("%s is a regular file\n", entry->d_name);
				
			char dstPath[1024];
			char *revName = reverseFileName(entry->d_name);
			snprintf(dstPath, sizeof(dstPath), "%s/%s", dst, revName);
			copyFile(srcPath, dstPath, fileStat);
		}
		else
		{
			printf("%s is not a regular file\n", entry->d_name);
		}
	}


	closedir(srcDir);
}

int main(int argc, char *argv[])
{
	if(argc != 2) printf("Invalid number of arguments\n");
	openDir(argv[1]);
	return 0;
}
