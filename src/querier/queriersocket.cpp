#include "queriersocket.h"
QuerierSocket::QuerierSocket()
	:Client()
	{

	}
QuerierSocket::QuerierSocket(const std::string addr)
	:Client(addr)
	{

	}
QuerierSocket::QuerierSocket(const int port)
	:Client(port)
	{

	}
QuerierSocket::QuerierSocket(const std::string addr,const int port)
	:Client(addr,port)
	{

	}
ssize_t QuerierSocket::sendFpmax(const std::string header, const std::string filedataname){
	//send fpmax
	std::string  packet;
        packet=createPacket(FPMAX_MSG,header,filedataname);
        ssize_t size=Send(socket,packet);

	int type;
        std::string data;
        std::string re_packet;
        std::string re_header;
        Receive(socket,re_packet);
        type=classifyPacket(re_packet,re_header,data);
        if(type!=ACCEPT)
                return -1;	
	
	return size;

}
ssize_t QuerierSocket::sendPubKey(const std::string header,const std::string filedataname){

	//send public key
	std::string  packet;
	packet=createPacket(PUBKEYSTORE_MSG,header,filedataname);
	ssize_t size=Send(socket,packet);
	

	//receive acceptance or refuse
	int type;
	std::string data;
	std::string re_packet;
	std::string re_header;
	Receive(socket,re_packet);
	type=classifyPacket(re_packet,re_header,data);
	if(type!=ACCEPT)
		return -1;


	return size;

}
ssize_t QuerierSocket::sendPSICARequest(const std::string header){

	std::string packet;
	std::string null;
	packet=createPacket(JOINREQUEST_MSG,header,null);
	ssize_t size=Send(socket,packet);

	//receive acceptance or refuse
	int type;
	std::string data;
	std::string re_packet;
	std::string re_header;
	Receive(socket,re_packet);
	type=classifyPacket(re_packet,re_header,data);
	if(type!=ACCEPT)
		return -1;

	return size;

}
ssize_t QuerierSocket::downloadNmax(const std::string header,std::string& data){
	std::string packet;
	std::string null;
	packet=createPacket(GETNMAX,header,null);
	Send(socket,packet);

	int type=-1;
	std::string re_packet;
	std::string re_header;
	ssize_t size=Receive(socket,re_packet);
	type=classifyPacket(re_packet,re_header,data);
	if(type==GETNMAX)
		return size;
	else
		return -1;
}
ssize_t QuerierSocket::downloadResult(const std::string header,std::string& data){

	std::string packet;
	std::string null;
	packet=createPacket(RESULTREQUEST_MSG,header,null);
	Send(socket,packet);

	//receive acceptance or refuse
	int type=-1;
	std::string re_packet;
	std::string re_header;
	ssize_t size=Receive(socket,re_packet);
	type=classifyPacket(re_packet,re_header,data);

	if(type==ENRESULT)
		return size;
	else
		return -1;

}
