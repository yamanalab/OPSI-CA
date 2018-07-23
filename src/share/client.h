#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <sys/types.h> /* connect, inet_addr, read, socket, write */ 
#include <sys/socket.h> /* connect, inet_addr, shutdown, socket */
#include <stdlib.h>/*exit*/
#include <string.h>/*memset*/
#include <netinet/in.h> /* inet_addr */
#include <arpa/inet.h> /* htons, inet_addr */
#include <sys/uio.h> /* read, write */ 
#include <unistd.h> /* close, read, write */
#include "socketlib.h"

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 12346
#define EXIT_MSG "EXIT"


class Client{

protected:
	int socket;
	struct sockaddr_in sa;
	const std::string server_addr;
	const int server_port;
	void setUp();

public:
	Client();
	Client(const std::string addr);
	Client(const int port);
	Client(const std::string addr,const int port);
	void connect();
	void disconnect();

};

#endif
