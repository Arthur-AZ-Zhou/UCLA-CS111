#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    for (int i = 0; i < argc; i++) {
        printf("argument %d is %s\n", i, argv[i]);
    }

    execlp("ls", "ls", "-l", NULL);

    return 0;
}