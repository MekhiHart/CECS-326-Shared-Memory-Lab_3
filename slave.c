	#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
using namespace std;

struct CLASS {
	int index;			// index to next available response slot
	int response[10];	// each child writes its child number here
};// how to implement struct into this??

int main(int argc, char **argv){
	// first argument: shm name
	// second argument: child idx
	// third argument: size

	int shm_fd;
	struct CLASS *shm_base;
	struct CLASS *shm_ptr;
	printf("Slave begins execution\n");
	printf("I am child number %s, received shared memory name %s\n",argv[1],argv[0]);

	shm_fd = shm_open(argv[0], O_RDWR, 0666);
	if (shm_fd == -1) {// Shared memory failed
		printf("cons: Shared memory failed: %s\n", strerror(errno));
	 	exit(1);
	}
	shm_base = (struct CLASS *) mmap(0, 4096, PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (shm_base == MAP_FAILED) { // map failed
		printf("cons: Map failed: %s\n", strerror(errno));
 		/* close and unlink */
		exit(1);
 	} 
	printf("I am child number %s, and the next empty slot is %d\n",argv[1],shm_base->index);
	// writes into shared memory
//	shm_base->index += 1; // increases next index
	shm_base->response[0] = 420; // test
	printf("I have written my child number to shared memory\n");
	
	shm_unlink(argv[0]); // unlinks shm_memory
	printf("Slave closed access to shared memory and terminated\n");
	exit(1);
}
