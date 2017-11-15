#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/dispatch.h>
#include <sys/mman.h>



void *send_msg(void *arg)
{

}

int main(int argc, char const *argv[])
{
	struct pid_data {
		pthread_mutex_t pid_mutex;
		pid_t pid;
	};

	int pid;

	int data[255];
	int buffer[255];

	int shm = shm_open("/sharepid", O_RDWR , S_IRWXU);


	pthread_mutex_lock(ptr->pid_mutex);
	pid = ptr->pid;
	printf("%d\n", pid);
	pthread_mutex_unlock(ptr->pid_mutex);
	

	pthread_t thread[4];
	for (int i = 0; i < 4; ++i)
	{
		pthread_create(&thread[i], NULL, send_msg, &pid);
	}


	// ass B 
	// int channelId = ConnectAttach(0, pid, 1, 0, 0);
	// MsgSend(channelId, data, sizeof(data), buffer, sizeof(buffer));

	// //after data received
	// ConnectDetach(channelId);

	return 0;
}