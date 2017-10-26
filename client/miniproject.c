#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>

#include "miniproject.h"

const double Kp =  10.0;
const double Ki = 800.0;
double period = .005;

struct udp_conn myConn;

/* Shared */
int counter = 0;
pthread_mutex_t mutex;
pthread_cond_t cond_var;

typedef enum state {
	Tcompute = 0,
	Texit
} Tstate;


Tstate PIDstate = Tcompute;
pthread_mutex_t PIDmutex;
pthread_cond_t PIDcond_var;
double recVal;


int udp_init_client(struct udp_conn *udp, int port, char *ip)
{
	struct hostent *host;

	if ((host = gethostbyname(ip)) == NULL) return -1;

	udp->client_len = sizeof(udp->client);
	// define servers
	memset((char *)&(udp->server), 0, sizeof(udp->server));
	udp->server.sin_family = AF_INET;
	udp->server.sin_port = htons(port);
	bcopy((char *)host->h_addr, (char *)&(udp->server).sin_addr.s_addr, host->h_length);

	// open socket
	if ((udp->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) return udp->sock;

	return 0;
}

int udp_send(struct udp_conn *udp, char *buf, int len)
{
	return sendto(udp->sock, buf, len, 0, (struct sockaddr *)&(udp->server), sizeof(udp->server));
}

int udp_receive(struct udp_conn *udp, char *buf, int len)
{
	int res = recvfrom(udp->sock, buf, len, 0, (struct sockaddr *)&(udp->client), &(udp->client_len));

	return res;
}

void udp_close(struct udp_conn *udp)
{
	close(udp->sock);
	return;
}

int clock_nanosleep_my(struct timespec *next)
{
	struct timespec now;
	struct timespec sleep;

	// get current time
	clock_gettime(CLOCK_REALTIME, &now);

	// find the time the function should sleep
	sleep.tv_sec = next->tv_sec - now.tv_sec;
	sleep.tv_nsec = next->tv_nsec - now.tv_nsec;

	// if the nanosecon is below zero, decrement the seconds
	if (sleep.tv_nsec < 0)
	{
		sleep.tv_nsec += 1000000000;
		sleep.tv_sec -= 1;
	}

	// sleep
	nanosleep(&sleep, NULL);

	return 0;
}

void timespec_add_us(struct timespec *t, long us)
{
	// add microseconds to timespecs nanosecond counter
	t->tv_nsec += us*1000;

	// if wrapping nanosecond counter, increment second counter
	if (t->tv_nsec > 1000000000)
	{
		t->tv_nsec -= 1000000000;
		t->tv_sec += 1;
	}
}

double PIDcontroller(double output, double *period)
{
	double reference = 1.0;

	double error = reference - output;
	double integral = integral + (error * *period);
	double result = Kp * error + Ki * integral;
	return result;
}

// void* PIDcontroller2(void* rank) {
//   long my_rank = (long) rank;
  
//   while(1) {
    
//     pthread_mutex_lock(&PIDmutex);
    
//     pthread_cond_wait(&PIDcond_var, &PIDmutex);
    
//     pthread_mutex_unlock(&PIDmutex);
    
//     if(PIDstate == Tcompute){
//       printf(" PID: doing some work.\n");
//     }
//     else
//     	break;
//   }
//   printf(" PID: exitting.\n");
//   return(NULL);
// } 

void* PIDcontroller2(void* rank) {
  long my_rank = (long) rank;
  //printf(" PID: started.\n");
  double sendVal;
  char sendBuff[256];
  struct timespec waitTime;

  
  while(PIDstate == Tcompute) {
    //TODO add mutex
  	udp_send(&myConn, "GET", 4);
  	printf(" PID: started.\n");
    
    pthread_mutex_lock(&PIDmutex);
    
    pthread_cond_wait(&PIDcond_var, &PIDmutex);
    
    pthread_mutex_unlock(&PIDmutex);
    
    if(PIDstate == Tcompute){
        printf(" PID: recieved value: %lf.\n", recVal);
        sendVal = PIDcontroller(recVal, &period);

    	sprintf(sendBuff, "SET:%.3lf", sendVal);
    	printf(" PID: %s\n", sendBuff);
    	
        clock_gettime(CLOCK_REALTIME, &waitTime);
    	timespec_add_us(&waitTime, 2500);
    	clock_nanosleep_my(&waitTime);

        //TODO add mutex
    	udp_send(&myConn, sendBuff, strlen(sendBuff) + 1);

    	clock_gettime(CLOCK_REALTIME, &waitTime);
    	timespec_add_us(&waitTime, 2500);
    	clock_nanosleep_my(&waitTime);
    }
    else
    	break;
  }
  printf(" PID: exitting.\n");
  return(NULL);
} 


int main()
{
    char getSignal[] = "GET";
    char recieveBuff[256];
    double sendVal;
    //double period = .005;


    //pthread_create(&signal, NULL, ThreadFunc, (void*) NULL);


 //    printf("%lu\n", strlen(getSignal));
    //struct udp_conn myConn;
	udp_init_client(&myConn, 9999, "127.0.0.1");
	udp_send(&myConn, "START", strlen("START") + 1);

	pthread_mutex_init(&PIDmutex, NULL);
    pthread_cond_init(&PIDcond_var, NULL);


    pthread_t controller, signal;
    pthread_create(&controller, NULL, PIDcontroller2, (void*) NULL);

	
	struct timespec waitTime;
	clock_gettime(CLOCK_REALTIME, &waitTime);

    //for(double time; time < 500.0; time += )
    for(int i = 0; i < 100; i++)
    {
     	//udp_send(&myConn, "GET", 4);
 //    	printf("this is i: %d\n", i);

     	udp_receive(&myConn, recieveBuff, 256);

    	
 //    	printf("%s\n", recieveBuff);
     	printf("main: recieved string: %s.\n", recieveBuff);
     	pthread_mutex_lock(&PIDmutex);
     	sscanf (&(recieveBuff[8]),"%lf",&recVal);
     	pthread_cond_signal(&PIDcond_var);
     	pthread_mutex_unlock(&PIDmutex);
    	
 //    	sendVal = PIDcontroller(recVal, &period);

 //    	sprintf(recieveBuff, "SET:%.3lf", sendVal);
 //    	printf("%s\n", recieveBuff);
 //    	timespec_add_us(&waitTime, 2500);
    	

 //    	clock_nanosleep_my(&waitTime);

 //    	udp_send(&myConn, recieveBuff, strlen(recieveBuff) + 1);

    	
     	//timespec_add_us(&waitTime, 5000);
    	

     	//clock_nanosleep_my(&waitTime);
     }
    udp_send(&myConn, "STOP", 5);



    // sleep(3);
    // printf("main: recieved value.\n");
    // pthread_mutex_lock(&PIDmutex);
    // pthread_cond_signal(&PIDcond_var);
    // pthread_mutex_unlock(&PIDmutex);

    //sleep(2);
    printf("main: exitting thread.\n");
    pthread_mutex_lock(&PIDmutex);
    PIDstate = Texit;
    pthread_cond_signal(&PIDcond_var);
    pthread_mutex_unlock(&PIDmutex);



    pthread_join(controller, NULL);
    //pthread_join(signal, NULL);
	return EXIT_SUCCESS;
}
