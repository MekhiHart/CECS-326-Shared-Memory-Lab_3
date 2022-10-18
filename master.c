#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <cstdio>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
using namespace std;


int main(int argc, char** argv) {
	pid_t cpid; // fork id
	int shm_fd;
	int num_children = atoi(argv[1]); // takes argv[1] from char** to int
	const int SIZE = 4096;
	const char *name = argv[2]; // shm name
	char *shm_base; // base address of base memory
	char *shm_ptr; // moveable ptr
	struct CLASS {
		int index;			// index to next available response slot
		int response[10];	// each child writes its child number here
	};
	// struct is what you write into the shared memory

	printf("Master begins execution\n");
	printf("This is %d\n", getpid());
	printf("Num children: %d. shm name: %s\n",num_children,name);
	shm_fd = shm_open(name,O_CREAT | O_RDWR,0666); // creates a shared memory object and returns an int file descriptor

	if (shm_fd == -1){ // Creating shared memory fails
		printf("Shared memory failed\n");
		exit(1);
	}

	ftruncate(shm_fd, SIZE); // configures size of the shared memory object
	shm_base = (char *)  mmap(0,SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd,0); // associates memory segment into return pointer
	if (shm_base == MAP_FAILED){
		printf("Map failed");
		exit(1);
	}
	
	
	for (int i=0; i < num_children; i++){ //creating child processes
		cpid = fork();
		char childNum[5];
		sprintf(childNum,"%d",i + 1);
		if (cpid == 0){ // child process
			execlp("./slave",name,childNum,NULL);

		}
	}
	while(wait(NULL) != -1); // blocks code until all children have terminated
	printf("Master received termination signals from all %d child processes",num_children);
	
	shm_unlink(name); // unlinks shared memory
	exit(0); // terminates program
}
