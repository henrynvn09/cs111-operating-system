#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include <errno.h>

int main(int argc, char *argv[])
{

	for (int i = 1; i < argc; i++) {

		int fds[2];

		// create n-1 pipes
		pipe(fds);
		if (pipe(fds) == -1) {
			perror("max file descriptors");
			return 1;
		}

		int pid = fork();
		if (pid == -1) {
			perror("max limit processes");
			return 1;
		}


		// if child process
		if (pid == 0){
			// close read end of the pipe
			close(fds[0]);

			// redirect stdout to pipe in except for last one
			if (i < argc - 1) {
				dup2(fds[1], STDOUT_FILENO);
				close(fds[1]);
			}

			int return_code = execlp(argv[i], argv[i], NULL);

			// close the write end of the pipe
			close(fds[1]);

			if (return_code == -1) {
				errno = EINVAL;
				//perror(argv[i]);
				exit(EINVAL);
			}
			exit(0);

		}
		else {
			// If parent process
			
			int status;
			wait(&status);
			if (status != 0) {
				fprintf(stderr, "Error: Invalid argument\n");
				exit(EINVAL);
			}


			// close the write end of the pipe
			close(fds[1]);

			// redirect pipe OUTPUT to STDIN
			dup2(fds[0], STDIN_FILENO);

			// close the read end of the pipe
			close(fds[0]);
		}
	}


	// wait for all children to finish
	int status;
	while ((wait(&status)) > 0) {
		if (status != 0) {
			return status;
		}
	}

	//printf("no error exit 0 from parent process");
	
	return 0;
}
