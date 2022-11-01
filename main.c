/********************
 * Liam Todd
 * This is the main code file that runs the shell
 * *****************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>

void changeDIR(char*);

int main() {
	char* readstr = malloc(2048 * sizeof(char));
	memset(readstr, '\0', 2048);

	size_t buffer_size = 2048;
	size_t bytes_read = -5;			//variables needed for getline command

	while (strncmp(readstr, "exit", 4) != 0) {
		memset(readstr, '\0', 2048);	//reset the readstr for each pass through loop
		printf(": ");
		fflush(stdout);
		bytes_read = getline(&readstr, &buffer_size, stdin);
	
		if (strncmp(readstr, "cd", 2) == 0)  
			changeDIR(readstr);
	}

	free(readstr);
	readstr = NULL;				//free dynamic mem
	return 0;
}

//this function will change to the desired directory
void changeDIR(char* tempstr) {
	char* desiredDIR = malloc((strlen(tempstr) + 1) * sizeof(char));
	strncpy(desiredDIR, tempstr, strlen(tempstr) - 1);	
	desiredDIR += 3;			//set pointer to user's input, but after the 'cd', will store name of directory

	int dirint = -5;

	DIR* directory = opendir(".");
	
	char path[100];
//	printf("pwd: %s\n", getcwd(path, sizeof(path)));

	dirint = chdir(desiredDIR); 
	if (dirint != 0) {
		printf("error opening to directory: %s\n", desiredDIR);
		fflush(stdout);
	}

//	printf("pwd: %s\n", getcwd(path, sizeof(path)));
	closedir(directory);

	desiredDIR -= 3;
	free(desiredDIR);
	desiredDIR = NULL;
}
