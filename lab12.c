#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

//Does the child stuff
void child(int pipefd[2], char * largs[]);
//Does the parent stuff
void parent(int pipefd[2], char * rargs[]);

int main(int argc, char * argv[]) {
	//find the pipe arg
	//section off two parts( 1:p, p+1:argc)
	//
	int i = 1;
	char * largs[argc];
	char * rargs[argc];
	while (i < argc && strcmp(argv[i], "-pipe") != 0) {
		i++;
	}
	if (i == argc) {
		printf("No pipe found!\n");
		return 0;
	} else {
		for (int j = 1; j < i; j++) {
			largs[j - 1] = argv[j];
		}
		largs[i - 1] = NULL;
		for (int j = 1; j < argc - i; j++) {
			rargs[j - 1] = argv[i + j];
			//printf("look: %s\n", rargs[j - 1]);
		}
		//printf("look: %s and i: %d\n", rargs[argc - i - 2], argc - i - 1);
		rargs[argc - i - 1] = NULL;
	}

	int pipefd[2];
	int p = pipe(pipefd);
	if (p < 0) {
		printf("Pipe failed\n");
		return 0;
	}

	int pid = fork();
	if (pid < 0) {
		printf("Fork failed\n");
	} else if (pid == 0) {
		child(pipefd, largs);
	} else {
		parent(pipefd, rargs);
	}

	return 0;
}


void child(int pipefd[2], char * largs[]) {
	close(pipefd[0]); // don't need that
	dup2(pipefd[1], 1); // Write to the pipe instead
	execvp(largs[0], largs);
}

void parent(int pipefd[2], char * rargs[]) {
	close(pipefd[1]); // close the writing end, we don't need that
	dup2(pipefd[0], 0); // Read input from pipe instead
	int status;
	wait(&status);
	execvp(rargs[0], rargs);
}
