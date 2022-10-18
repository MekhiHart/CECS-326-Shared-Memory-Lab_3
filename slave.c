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

int main(int argc, char **argv){
	// first argument: shm name
	// second argument: child idx
	// third argument: size
	struct CLASS {
		int index;			// index to next available response slot
		int response[10];	// each child writes its child number here
	};// how to implement struct into this??

	int shm_fd;
	char *shm_base;
	char *shm_ptr;
	printf("Slave begins execution\n");
	printf("I am child number %s, received shared memory name %s\n",argv[1],argv[0]);

	shm_fd = shm_open(argv[0], O_RDONLY, 0666);
	if (shm_fd == -1) {// Shared memory failed
		printf("cons: Shared memory failed: %s\n", strerror(errno));
	 	exit(1);
	}
	shm_base = (char *) mmap(0, 4096, PROT_READ, MAP_SHARED, shm_fd, 0);
	if (shm_base == MAP_FAILED) { // map failed
		printf("cons: Map failed: %s\n", strerror(errno));
 		/* close and unlink */
		exit(1);
 	} 
	shm_ptr = shm_base;
	// writes into shared memory
	const char *test="hello";
	sprintf(shm_ptr,"%s",test); // error here
	printf("adds into shm_ptr of test content\n");
	shm_ptr += strlen(test);
	printf("I have written my child number to shared memory\n");
	
	shm_unlink(argv[0]); // unlinks shm_memory
	printf("Slave closed access to shared memory and terminated\n");
	exit(1);
}
