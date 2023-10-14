#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    // for (int i = 0; i < argc; i++) {
    //     printf("argument %d is %s\n", i, argv[i]);
    // }

	int i;

	for (i = 1; i < argc - 1; i++) {
		int fds[2];
		pid_t childpid;

		childpid = fork();

		if (childpid == 0) {
			dup2(fds[1], 1); //link stdout to parent
			execlp(argv[i], argv[i], NULL);
			perror("execlp");
			return 1;
		}

		int status;

		waitpid(childpid, &status, 0);
		dup2(fds[0], 0); //link stdout of previous child to stdin
		close(fds[1]);
	}

	execlp(argv[i], argv[i], NULL);
	perror("exec");
	abort();

	// int i;

	// for (i = 1; i < argc - 1; i++) {
	// 	int fds[2];
	// 	pipe(fds);

	// 	if (!fork()) {
	// 		dup2(fds[1], 1); //link stdout to parent
	// 		execlp(argv[i], argv[i], NULL);
	// 		perror("exec");
	// 		abort();
	// 	}

	// 	dup2(fds[0], 0); //link stdout of previous child to stdin
	// 	close(fds[1]);
	// }

	// execlp(argv[i], argv[i], NULL);
	// perror("exec");
	// abort();

	// int returnCode = system(argv[1]);

	// if (returnCode == 0) {
	// 	printf("Command executed successfully.\n");
	// } else {
	// 	printf("Command failed w/ return code %d.\n", returnCode);
	// }

	// printf("All files in executable path: ");
	// execlp("ls", "ls", NULL); //lists out all files in ls' executable path

    return 0;
}
