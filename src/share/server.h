#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string.h> //strerror
#include <errno.h> //erno
#include <arpa/inet.h> //htons
#include <stdio.h> //fprintf, perror
#include <stdlib.h> //exit
#include <string.h> //memset,strlen,strstr
#include <sys/types.h> //accept,bind,setsocokopt, socket,write
#include <sys/socket.h> //accept,bind,listen,setsockopt, shutdown, socket
#include <sys/uio.h> //write
#include <unistd.h> //close,write
#include "socketlib.h"

#define SERVER_ADDR "127.0.0.1"

#define MAXNCLIENTS 10
#define NQUEUESIZE 5
#define SERVER_PORT 12346

// enum{
// 	SERVER_PORT = 12345,
// 	NQUEUESIZE = 5,
// 	MAXNCLIENTS = 10,
// };

class Server{


protected:
	int socket;
	int server_port;
	int clients[MAXNCLIENTS];//list of connected clients
	int nclients;
	void setUp(); 

public:

	Server();
	Server(const int port);
	void receive(int& client, int& type,std::string& packet, std::string& header, std::string& data);
	


};


#endif
