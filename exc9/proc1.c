#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/dispatch.h>
#include <sys/mman.h>

int main(int argc, char const *argv[])
{
	struct pid_data {
		pthread_mutex_t pid_mutex;
		pid_t pid;
	};

	pthread_mutex_attr_t myattr;

 	int buffer[255];  
 	int data[255];

	int shm = shm_open("/sharepid", O_RDWR | O_CREAT, S_IRWXU);
	ftruncate(shm, sizeof(struct pid_data));
	struct pid_data *ptr = mmap(0, sizeof(struct pid_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);			 

	pthread_mutexattr_init(&myattr);
	pthread_mutexattr_setpshared(&myattr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&ptr->pid_mutex, &myattr);
 
	pthread_mutex_lock(ptr->pid_mutex);
	ptr->pid = getpid();
	pthread_mutex_unlock(ptr->pid_mutex);

	// ass B
	int channelId = ChannelCreate(0);
	int receiveId = MsgReceive(channelId, buffer, sizeof(buffer), NULL);

	// after job
	MsgReply(receiveId, 0, data, sizeof(data));
	return 0;
}