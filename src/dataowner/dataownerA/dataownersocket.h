#ifndef DATAOWNERSOCKET_H
#define DATAOWNERSOCKET_H

#include "client.h"

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 12346
#define EXIT_MSG "EXIT"

class DataOwnerSocket:public Client{
public:
	DataOwnerSocket();
	DataOwnerSocket(const std::string addr);
	DataOwnerSocket(const int port);
	DataOwnerSocket(const std::string addr, const int port);
	ssize_t sendData(const std::string header,const std::string filedataname);
	ssize_t downloadNmax(const std::string header,std::string& data);

};

#endif
