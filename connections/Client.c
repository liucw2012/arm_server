#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <sys/types.h>
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>


#define RCVBUFSIZE 32   /* Size of receive buffer */
#define MAX_CONN   500

int sock_array[MAX_CONN];

void DieWithError(char *errorMessage)
{
	printf("%s\n", errorMessage);
	exit(1);
}

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    int i, one = 1;
    struct sockaddr_in echoServAddr; /* Echo server address */
    unsigned short echoServPort;     /* Echo server port */
    char *servIP;                    /* Server IP address (dotted quad) */
    char *echoString;                /* String to send to echo server */
    char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */
    unsigned int echoStringLen;      /* Length of string to echo */
    int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv() 
                                        and total bytes read */

    if ((argc < 3) || (argc > 4))    /* Test for correct number of arguments */
    {
       fprintf(stderr, "Usage: %s <Server IP> <Echo Word> [<Echo Port>]\n",
               argv[0]);
       exit(1);
    }

    servIP = argv[1];             /* First arg: server IP address (dotted quad) */
    if (argc == 3)
        echoString = argv[2];
    else 
        echoString = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

    if (argc == 4)
        echoServPort = atoi(argv[3]); /* Use given port, if any */
    else
        echoServPort = 7;  /* 7 is the well-known port for the echo service */

    for (i = 0; i < MAX_CONN; i++) {
	    /* Create a reliable, stream socket using TCP */
	    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		DieWithError("socket() failed");

	    /* Construct the server address structure */
	    memset(&echoServAddr, 0, sizeof(echoServAddr));     /* Zero out structure */
	    echoServAddr.sin_family      = AF_INET;             /* Internet address family */
	    echoServAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
	    echoServAddr.sin_port        = htons(echoServPort); /* Server port */

	    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	    /* Establish the connection to the echo server */
	    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
		DieWithError("connect() failed");
	    sock_array[i] = sock;
	    usleep(6*1000);
    }

    echoStringLen = strlen(echoString);          /* Determine input length */

    while (1) {
	    for (i = 0; i < MAX_CONN; i++) {
	    	    usleep(10*1000);
		    /* Send the string to the server */
		    sock = sock_array[i];
		    if (send(sock, echoString, echoStringLen, 0) != echoStringLen)
			DieWithError("send() sent a different number of bytes than expected");
	    }
    }

    printf("\n");    /* Print a final linefeed */

    for (i = 0; i < MAX_CONN; i++) {
        sock = sock_array[i];
        if (sock > 0)
            close(sock);
    }

    exit(0);
}
