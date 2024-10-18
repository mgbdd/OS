#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

void createDir(char *dirPath)
{
	if(mkdir(dirPath, 0755) == -1)
	{
		perror("mkdir");
		printf("Directory %s wasn't created\n", dirPath);
		return;
	}
	else printf("Directory '%s' was created\n", dirPath);
}


void readDir(char *dirPath)
{
	DIR *dir = opendir(dirPath);
	if(dir == NULL)
	{
		printf("Directory %s wasn't found\n", dirPath);
		perror("opendir");
		return;
	}

	struct dirent *entry;
	while((entry = readdir(dir)) != NULL)
	{
		printf("%s\n", entry->d_name);
	}
	closedir(dir);
}

void deleteDir(char *dirPath)
{
	DIR *dir = opendir(dirPath);
	if(dir == NULL)
	{
		printf("Directory %s wasn't found\n",dirPath);
		perror("opendir");
		return;
	}
	struct dirent *entry;
	char filePath[1024];

	while((entry = readdir(dir)) != NULL)
	{
		if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
		snprintf(filePath, sizeof(filePath), "%s/%s",  dirPath, entry->d_name);
		if(entry->d_type == DT_DIR) deleteDir(filePath);
		else
		{
			if(remove(filePath) != 0) perror("remove");
			else printf("%s was deleted\n", entry->d_name);
		}
	}
	if(rmdir(dirPath) != 0) perror("rmdir");
	else printf("Directory %s was deleted\n", dirPath);
	closedir(dir);
}

void createFile(char *filePath)
{	
	FILE *file = fopen(filePath, "w");
	if(file == NULL) 
	{
		printf("File %s wasn't opened\n", filePath);
		perror("fopen");
		return;
	}

	if (chmod(filePath, 0777))
	{
        	perror("chmod");
        	fclose(file);
        	return;
    	}	

	fclose(file);
}

void readFile(char *filePath)
{
	FILE *file = fopen(filePath, "r");
	if(file == NULL) 
	{
		printf("File %s wasn't found\n", filePath);
		perror("fopen");
		return;
	}

	char buffer[256];
	while(fgets(buffer, sizeof(buffer), file) != NULL)
	{
		printf("%s", buffer);
	}
	
	fclose(file);
	return;
}

void deleteFile(char *filePath)
{
	FILE *file = fopen(filePath,"w");
	if(file == NULL)
	{
		printf("File %s wasn't found\n", filePath);
		perror("fopen");
		return;
	}
	if(remove(filePath) != 0) perror("remove");
	else printf("File %s was deleted\n", filePath);
}

void createSymbolLink(char *filePath)
{
	char linkName[256];
	snprintf(linkName, sizeof(linkName), "symbol_link_%s", filePath);

	if(symlink(filePath, linkName) != 0)
	{
		printf("Symbol link of file %s wasn't created\n", filePath);
		perror("symlink");
		return;
	}
}

void readSymbolLink(char *linkName)
{
	char targetPath[1024];
	ssize_t len = readlink(linkName, targetPath, sizeof(targetPath) - 1);
	if(len == -1)
	{
		printf("Link %s wasn't read\n", linkName);
		perror("readlink");
	}
	targetPath[len] = '\0';
	printf("%s", targetPath);
}

void readLinkedFile(char *linkName)
{
	char targetPath[1024];
	ssize_t len = readlink(linkName, targetPath, sizeof(targetPath) - 1);
	if(len == -1)
	{
		printf("Link %s wasn't read\n", linkName);
		perror("readlink");
	}
	targetPath[len] = '\0';

	FILE *file = fopen(targetPath, "r");
	if(file == NULL)
	{
		printf("File %s wasn't opened\n", targetPath);
		perror("fopen");
		return;
	}

	readFile(targetPath);
}

void deleteSymbolLink(char *linkName)
{
	if(remove(linkName) != 0) perror("remove");
	else printf("Link %s was deleted\n", linkName);
}

void createHardLink(char *filePath)
{
	char linkName[256];
	snprintf(linkName, sizeof(linkName), "hard_link_%s", filePath);
	if(link(filePath, linkName) == -1)
	{
		printf("Hard link %s wasn't created\n", linkName);
		perror("link");
		return;
	}
}

void deleteHardLink(char *linkPath)
{
	if (unlink(linkPath) == -1) {
        perror("unlink");
        return;
    }	
}


void readRights(char *filePath)
{
    struct stat fileStat;

    if (stat(filePath, &fileStat) == -1) {
        perror("stat");
        return;
    }

    printf("Rights: %s: ", filePath);

    // Права доступа для владельца
    printf("%c", (fileStat.st_mode & S_IRUSR) ? 'r' : '-');
    printf("%c", (fileStat.st_mode & S_IWUSR) ? 'w' : '-');
    printf("%c", (fileStat.st_mode & S_IXUSR) ? 'x' : '-');

    // Права доступа для группы
    printf("%c", (fileStat.st_mode & S_IRGRP) ? 'r' : '-');
    printf("%c", (fileStat.st_mode & S_IWGRP) ? 'w' : '-');
    printf("%c", (fileStat.st_mode & S_IXGRP) ? 'x' : '-');

    // Права доступа для других
    printf("%c", (fileStat.st_mode & S_IROTH) ? 'r' : '-');
    printf("%c", (fileStat.st_mode & S_IWOTH) ? 'w' : '-');
    printf("%c", (fileStat.st_mode & S_IXOTH) ? 'x' : '-');

    printf("\n");

    printf("Number of hard links of %s: %ld\n", filePath, fileStat.st_nlink);
}

void changeRights(char *filePath, char *perms)
{
	mode_t mode = 0;

    // Разбор строки с правами доступа
    for (int i = 0; permissions[i]; i++) {
        switch (permissions[i]) {
            case 'r':
                mode |= S_IRUSR | S_IRGRP | S_IROTH;
                break;
            case 'w':
                mode |= S_IWUSR | S_IWGRP | S_IWOTH;
                break;
            case 'x':
                mode |= S_IXUSR | S_IXGRP | S_IXOTH;
                break;
            case '-':
                break;
            default:
                printf("Неизвестный символ в строке прав доступа: '%c'\n", permissions[i]);
                return 1;
        }
    }

    // Установка прав доступа
    if (chmod(filename, mode) != 0) {
        printf("Ошибка установки прав доступа к файлу %s\n", filename);
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("Not enough arguments\n");
		return 0;
	}
	char *path = argv[1];
	char *rights = (argc == 3) ? argv[2] : "rwxrwxrwx"; 	
	
	char linkPath[256];
	ssize_t len;
	len = readlink("/proc/self/exe", linkPath,sizeof(linkPath) - 1);
	if(len == -1)
	{
		perror("readlinl");
		return 0;
	}
	linkPath[len] = '\0';
	
	char pos;
	for(int i = len - 1; i >= 0; i--)
	{
		if(linkPath[i] == '/') 
		{
			pos = i;
		        break;
		}	       
	}
	char linkName[len - pos - 1];
	for(int i = 0; i < len - pos - 1; i++)
	{
		linkName[i] = linkPath[i + pos + 1];
	}
	linkName[len - pos - 1] = '\0';
	printf("current link: %s\n", linkName);

	if(strcmp(linkName, "createDir") == 0) createDir(path);
	else if(strcmp(linkName, "readDir") == 0) readDir(path);
	else if(strcmp(linkName, "deleteDir") == 0) deleteDir(path);
	else if(strcmp(linkName, "createFile") == 0) createFile(path);
	else if(strcmp(linkName, "readFile") == 0) readFile(path);
	else if(strcmp(linkName, "deleteFile") == 0) deleteFile(path);
	else if(strcmp(linkName, "createSymbolLink") == 0) createSymbolLink(path);
	else if(strcmp(linkName, "readSymbolLink") == 0) readSymbolLink(path);
	else if(strcmp(linkName, "readLinkedFile") == 0) readLinkedFile(path);
	else if(strcmp(linkName, "deleteSymbolLink") == 0) deleteSymbolLink(path);
	else if(strcmp(linkName, "createHardLink") == 0) createHardLink(path);
	else if(strcmp(linkName, "deleteHardLink") == 0) deleteHardLink(path);
	else if(strcmp(linkName, "readRights") == 0) readRights(path);
	else if(argc == 3 && strcmp(linkName, "changeRights") == 0) changeRights(path, rights);
	else printf("Not enough arguments for right changing\n");
}
