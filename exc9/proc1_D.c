#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/dispatch.h>
#include <sys/mman.h>

#include <sys/neutrino.h> /* for _msg_info */


// struct _msg_info {        /* _msg_info    _server_info */
//     uint32_t  nd;         /*  client      server */
//     uint32_t  srcnd;      /*  server      n/a */
//     pid_t     pid;        /*  client      server */
//     int32_t   tid;        /*  thread      n/a */
//     int32_t   chid;       /*  server      server */
//     int32_t   scoid;      /*  server      server */
//     int32_t   coid;       /*  client      client */
//     int32_t   msglen;     /*  msg         n/a */
//     int32_t   srcmsglen;  /*  thread      n/a */
//     int32_t   dstmsglen;  /*  thread      n/a */
//     int16_t   priority;   /*  thread      n/a */
//     int16_t   flags;      /*  n/a         client */ 
//     uint32_t  reserved;
// };

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

int main(int argc, char const *argv[])
{
	struct pid_data {
		pthread_mutex_t pid_mutex;
		pid_t pid;
	};

	pthread_mutex_attr_t myattr;

 	int buffer[255];  
 	int data[255];

 	struct _msg_info info; 

 	int counter = 0;

 	set_priority(10); 

	int shm = shm_open("/sharepid", O_RDWR | O_CREAT, S_IRWXU);
	ftruncate(shm, sizeof(struct pid_data));
	struct pid_data *ptr = mmap(0, sizeof(struct pid_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);			 

	pthread_mutexattr_init(&myattr);
	pthread_mutexattr_setpshared(&myattr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&ptr->pid_mutex, &myattr);
 
	pthread_mutex_lock(ptr->pid_mutex);
	ptr->pid = getpid();
	pthread_mutex_unlock(ptr->pid_mutex);

	// ass D 
	int receiveId;
	int priority;

	// change this
	int channelId = ChannelCreate(_NTO_CHF_FIXED_PRIORITY);
	// int channelId = ChannelCreate(0); 

	while(1)
	{
		receiveId = MsgReceive(channelId, buffer, sizeof(buffer), &info);
		printf("pid = %d, tid = %d \n", info.pid, info.tid);
		// after job
		priority = get_priority();
		printf("%d\n", priority);

		data = counter++;
		MsgReply(receiveId, 0, data, sizeof(data)); 
	}
	
	return 0;
}