#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#include <sys/epoll.h>
#include <fcntl.h>

#define MAXPENDING 1024 /* Maximum outstanding connection requests */
#define MAXFD	40000

/* Structure of arguments to pass to client thread */
struct ThreadArgs
{
    int clntSock;                      /* Socket descriptor for client */
};

struct epoll_event *array;
int epollFd;

void *ThreadMain(void *threadArgs)
{
    int count, ret, i, total = 0;
    char buff[1024];
    struct epoll_event *event;

    while (1) {
	    count = epoll_wait(epollFd, array, MAXFD, 1);
	    if (count > 0) {
		    for (i = 0; i < count; i++) {
			    event = array + i;
			    while ((ret = recv(event->data.fd, buff, sizeof(buff), MSG_DONTWAIT)) > 0) {
				    total += ret;
				    if (total >= 10485760) {
				    	printf("%d %d\n", getpid(), total);
					total = 0;
				    }
			    }
		    }
	    }
    }
    return (NULL);
}
int main(int argc, char *argv[])
{
    int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
    int opts, one = 1;
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned short echoServPort;     /* Server port */
    unsigned int clntLen;            /* Length of client address data structure */
    pthread_t threadID;              /* Thread ID from pthread_create() */
    struct epoll_event event;

    if (argc != 2)     /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);  /* First arg:  local port */

    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        printf("socket() failed");
      
    epollFd = epoll_create(1);
    if (epollFd <= 0)
	printf("epoll create failed");

    array = (struct epoll_event*)malloc(MAXFD * sizeof(struct epoll_event));
    if (!array)
	printf("alloc array failed\n");

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        printf("bind() failed");

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
        printf("listen() failed");
    
    if (pthread_create(&threadID, NULL, ThreadMain, (void *)&clntSock) != 0)
	    printf("pthread_create() failed\n");

    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        clntLen = sizeof(echoClntAddr);

        /* Wait for a client to connect */
        if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, 
                               &clntLen)) < 0)
            printf("accept() failed");

        /* clntSock is connected to a client! */

        //printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

	setsockopt(clntSock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	/* set NONBLOCK */
	opts = fcntl(clntSock, F_GETFL);
	opts |= O_NONBLOCK;
	fcntl(clntSock, F_SETFL, opts);
        /* Create client thread */
	event.events = EPOLLIN|EPOLLET;
	event.data.fd = clntSock;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clntSock, &event))
		printf("add cltSock fail\n");
    }
    /* NOT REACHED */
}
