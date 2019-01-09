/*! \file ECHOserverThreaded.c
    \brief TELNET Echo Server that replies the received text, but inverted
	\authors Ignacio Tamayo
	\date June 11th 2016
	\version 1.0
	
	This is a simple TELNET ECHO Server, that replies the received text but string inverted
	
	If it receives "aBcD", it replies "DcBa".
	If it receives "Q" or "q", connection with the client is closed
	
	Internally a new thread is created by each connection.
	As threads are created, they are detached
	
	Execute:
	* \code
	* ECHOserver <Port to Listen at>
	* \endcode
	
*/

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


#define MAX_WAITING_CLIENTS 	5
#define BUFFER_SIZE				256

#define WELCOM_MSG	"Welcome to the OHCE Server \n(Q/q) to quit\n"


/** Bind a given socket to a connection to hostname:port using TCP
 * \param sock is the socket
 * \param port is the port to bind to
 * \param hostname is the hostname where to bind and listen
\return  -1 if error, >0 if OK.
*/
int bind_tcp(int sock, int port, char* hostname);

/** Function that interacts with the client, doing the message exchange 
 * \param * client_socket is the pointer to a working socket for this connection
 */
void * echo_server(void * client_socket);

/** Signal handler
 * \param signal to receive
 */
void sig_handler(int signal);

int connections = 0;  //!< Counter of connections. Used to determine when there are no more connected clients and close the server

/** Main
 * \param argc
 * \param argv
 * \return 0 in case of success
 */
int main(int argc, char * argv[])
{
	int sock,client_sock, clilen;
	char buffer[BUFFER_SIZE];
	struct sockaddr_in  cli_addr;
	int  n, opt=1;
	int port;
	pthread_t thread;	// The thread

	//Capturing the port as parameter
	if(argc != 2)
	{
		printf("Wrong command\n");
		printf("ECHOserver <TCP port to liste to>\n");
		exit(9);
	}
	
	port =  atoi(argv[1]);
	 
	//Create socket
	sock = socket(AF_INET, SOCK_STREAM, 0); // Socket tipo INTERNET; STEAM=TCP
	if (sock < 0) { 	      perror("ERROR opening socket"); 	      return 11; 	}
	
	//Set option to re-use the socket if it is taken in TimeWait states or similar states
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(int));

	//Try to bind the socket
	if ( bind_tcp( sock , port , "0.0.0.0" ) == -1){ 	      perror("ERROR binding socket"); 	      return 12; 	}

	//Listen at the new socket for new connections
	listen(sock, MAX_WAITING_CLIENTS);
	clilen = sizeof(cli_addr);
	while(1)
	{
		
		
		client_sock = accept(sock, (struct sockaddr *)&cli_addr, &clilen);
		//---- Program stops here and does not go further until a connection comes in

		printf ("Client connected\n");

		//create thread for the client
		pthread_create(&thread, NULL, &echo_server, (void*)&client_sock);
		pthread_detach(thread);
		
	}

}


int bind_tcp(int sock, int port, char* hostname)
{
	/*
	//Get protocol TCP from the system /etc/protocols
	struct protoent * protocol;
	protocol = getprotobyname(TCP_PROTO_NAME)	

	//Get service from the system /etc/protocols
	struct servent * servicio;
	servicio = getservbyname(SERVICE,protocolo->p_name);
	*/

	// Get hostname by DNS system
	struct hostent * host;
	host = gethostbyname(hostname);

	//Get structure sockaddr_in ready
	struct sockaddr_in address;
	address.sin_family = AF_INET;	//This says that this is an internet socket
	address.sin_port = htons(port);	//This indicates the Port number we are using. We use HTONS because of byte order (just copy it this way)
	address.sin_addr.s_addr = ((struct in_addr *) (host->h_addr))->s_addr;  //Getting the IP address of the host

	//Naming of the socket and binding
	return bind ( sock,  (struct sockaddr*) &address, sizeof(struct sockaddr_in));	
}

void* echo_server(void * client_socket){
	int socket=*(int*)client_socket, received_bytes, data_size,i;
	char buffer[BUFFER_SIZE],c;
	

	bzero(buffer,BUFFER_SIZE);

	strcpy(buffer,WELCOM_MSG);
	data_size=strlen(buffer);

	send(socket,&buffer,data_size,0);

	bzero(buffer,BUFFER_SIZE);
	
	while(received_bytes=recv(socket,&buffer,BUFFER_SIZE,0))
	{
														//printf("%d : %s\n",socket, Request.message);
			i = 0;
			if (((buffer[0] == 'Q') || (buffer[0] == 'q') ) && (received_bytes==3))
			{
				close(socket);
				break;
			}
			if (received_bytes > 1)
			{
				printf ("Client said: %s", buffer);
				while(i<=(received_bytes-2)/2)
				{
					c = buffer[received_bytes-2-i] ;
					buffer[received_bytes-2-i] = buffer[i];
					buffer[i] = c;
					i++;
				}
			//buffer[received_bytes] = '\n' ;buffer[received_bytes+1] = '\r' ;
			}											//printf("Closing = %d\n",answer);
			send(socket,&buffer,received_bytes+2,0);
			bzero(buffer,BUFFER_SIZE);
	}
		
}
	
