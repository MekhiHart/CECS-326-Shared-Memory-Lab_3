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
	int index;			// index to next available response slot
	int response[10];	// each child writes its child number here
};// how to implement struct into this??

int main(int argc, char **argv){
	// first argument: shm name
	// second argument: child idx
	// third argument: size
	const int SIZE = 4096;
	int shm_fd;
	struct CLASS *shm_base;
	struct CLASS *shm_ptr;
	int arrIdx = atoi(argv[1]); // index needed to change;
	printf("Slave begins execution\n");
	printf("I am child number %s, received shared memory name %s\n",argv[1],argv[0]);

	shm_fd = shm_open(argv[0], O_RDWR, 0666);
	if (shm_fd == -1) {// Shared memory failed
		printf("cons: Shared memory failed: %s\n", strerror(errno));
	 	exit(1);
	}
	shm_base = (struct CLASS *) mmap(0, SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (shm_base == MAP_FAILED) { // map failed
		printf("cons: Map failed: %s\n", strerror(errno));
 		/* close and unlink */
		exit(1);
 	} 
	//printf("I am child number %s, and the next empty slot is %d\n",argv[1],shm_base->index);
	// writes into shared memory
	printf("Value being written into child num %s: %d",argv[1],arrIdx);
	shm_base->response[arrIdx - 1] = arrIdx; // test successful
	printf("I have written my child number to shared memory\n");
	
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

	//shm_unlink(argv[0]); // unlinks shm_memory
	printf("Slave closed access to shared memory and terminated\n");
	exit(1);
}
