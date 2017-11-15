#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/dispatch.h>
#include <sys/mman.h>

int set_priority(int priority)
{
	int policy;
	struct sched_param param;
	// check priority in range
	if (priority < 1 || priority > 63) return -1;
	// set priority
	pthread_getschedparam(pthread_self(), &policy, &param);
	param.sched_priority = priority;
	return pthread_setschedparam(pthread_self(), policy, &param);
}

int get_priority() 
{
	int policy;
	struct sched_param param;
	// get priority
	pthread_getschedparam(pthread_self(), &policy, &param);
	return param.sched_curpriority;
}


void *send_msg1(void *arg)
{
	set_priority(6); 

	int pid = *arg;

	int buffer[255];
	int data[255];

	int channelId = ConnectAttach(0, pid, 1, 0, 0);
	MsgSend(channelId, data, sizeof(data), buffer, sizeof(buffer));

	printf("%d\n", buffer);

	return 0;
}

void *send_msg2(void *arg)
{
	set_priority(5); 

	int pid = *arg;

	int buffer[255];
	int data[255];

	int channelId = ConnectAttach(0, pid, 1, 0, 0);
	MsgSend(channelId, data, sizeof(data), buffer, sizeof(buffer));

	printf("%d\n", buffer);

	return 0;
}

void *send_msg3(void *arg)
{
	set_priority(14); 

	int pid = *arg;

	int buffer[255];
	int data[255];

	int channelId = ConnectAttach(0, pid, 1, 0, 0);
	MsgSend(channelId, data, sizeof(data), buffer, sizeof(buffer));

	printf("%d\n", buffer);

	return 0;
}

void *send_msg4(void *arg)
{
	set_priority(13); 

	int pid = *arg;

	int buffer[255];
	int data[255];

	int channelId = ConnectAttach(0, pid, 1, 0, 0);
	MsgSend(channelId, data, sizeof(data), buffer, sizeof(buffer));

	printf("%d\n", buffer);

	return 0;
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
	
	set_priority(50);

	pthread_t thread[4];
	
	pthread_create(&thread[0], NULL, send_msg1, &pid);
	pthread_create(&thread[1], NULL, send_msg2, &pid);
	pthread_create(&thread[2], NULL, send_msg3, &pid);
	pthread_create(&thread[3], NULL, send_msg4, &pid);
	
	for (int i = 0; i < 4; ++i)
	{
		pthread_join(thread[i], NULL);
	}

	// ass B 
	// int channelId = ConnectAttach(0, pid, 1, 0, 0);
	// MsgSend(channelId, data, sizeof(data), buffer, sizeof(buffer));

	// //after data received
	// ConnectDetach(channelId);

	return 0;
}