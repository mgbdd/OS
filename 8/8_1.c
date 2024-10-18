#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {

	
	uid_t realUid = getuid();
    uid_t effectiveUid = geteuid();

    printf("\nReal user ID: %d\n", realUid);
    printf("Effective user ID: %d\n", effectiveUid);	

	char *path = argv[1];
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        printf("Error opening file\n");
        return 0;
    }

    char line[100];
    while (fgets(line, sizeof(line), file) != NULL) 
    {
        printf("%s", line);
    }

    fclose(file);

    return 0;
}
