#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
//using namespace std;

struct CLASS {
	int response[10];	// each child writes its child number here
	int index;
};// how to implement struct into this??

int main(int argc, char **argv){
	const int SIZE = sizeof(struct CLASS);
	int shm_fd;
	int *i; // local variable that will hold the value of the next free index
	struct CLASS *shm_base;
	int childNum = atoi(argv[1]); // index needed to change;
	printf("Slave begins execution\n");
	printf("I am child number %d, received shared memory name %s\n",childNum,argv[0]);

	shm_fd = shm_open(argv[0], O_RDWR, 0666); // opens shared memory file assoicated with the name given
	if (shm_fd == -1) {// Shared memory failed
		printf("cons: Shared memory failed: %s\n", strerror(errno));
	 	exit(1);
	}
	shm_base = (struct CLASS *) mmap(0, SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0); // returns a memory address of the assoicated file descriptor passed in with the instructions to write on it
	if (shm_base == MAP_FAILED) { // map failed
		printf("cons: Map failed: %s\n", strerror(errno));
 		/* close and unlink */
		exit(1); // terminates program
 	}
	printf("Next free index: %d\n",shm_base->index);
	printf("Slave acquires access to shared memory segment, and structures it according to struct CLASS\n");
	i = &(shm_base->index);
	printf("Slave copies index to a local variable i\n");
	// writes into shared memory
	//printf("Value being written into child num %s: %d",argv[1],arrIdx);
	shm_base->response[*i] = childNum;  
	printf("slave writes its child number in response[%d] in shared memory\n",*i);
	*i += 1; // increments shm_base index
	printf("slave increments index");
	// removes shared memory segment
	if (munmap(shm_base, SIZE) == -1) { // removes memory mapping 
 		printf("prod: Unmap failed: %s\n", strerror(errno));
 		exit(1);
 	}
	if (close(shm_fd) == -1) { // closes shared memory segment
 		printf("prod: Close failed: %s\n",
		strerror(errno));
 		exit(1);
	 }

	printf("slave closed access to shared memory and terminates\n");
	printf("slave exits\n");
	exit(1); // terminates program
}
