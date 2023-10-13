#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    for (int i = 0; i < argc; i++) {
        printf("argument %d is %s\n", i, argv[i]);
    }

	int returnCode = system(argv[1]);

	if (returnCode == 0) {
		printf("Command executed successfully.\n");
	} else {
		printf("Command failed w/ return code %d.\n", returnCode);
	}

	printf("All files in executable path: ");
	execlp("ls", "ls", NULL); //lists out all files in ls' executable path

    return 0;
}
