/********************
 * Liam Todd
 * This is the main code file that runs the shell
 * *****************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	}

	free(readstr);
	readstr = NULL;				//free dynamic mem
	return 0;
}
