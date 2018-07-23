#ifndef QUERIERSOCKET_H
#define QUERIERSOCKET_H

#include "client.h"

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 12346
#define EXIT_MSG "EXIT"


class QuerierSocket :public Client{

public:
	QuerierSocket();
	QuerierSocket(const std::string addr);
	QuerierSocket(const int port);
	QuerierSocket(const std::string addr,const int port);
	ssize_t sendFpmax(const std::string header,const std::string filedataname);
	ssize_t sendPubKey(const std::string header,const std::string filedataname);
	ssize_t sendPSICARequest(const std::string header);
	ssize_t downloadNmax(const std::string header,std::string& data);
	ssize_t downloadResult(const std::string header,std::string& data); 
};

#endif
